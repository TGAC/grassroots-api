/*  $Id: wn_main_loop.cpp 448239 2014-10-03 14:17:45Z kazimird $
 * ===========================================================================
 *
 *                            PUBLIC DOMAIN NOTICE
 *               National Center for Biotechnology Information
 *
 *  This software/database is a "United States Government Work" under the
 *  terms of the United States Copyright Act.  It was written as part of
 *  the author's official duties as a United States Government employee and
 *  thus cannot be copyrighted.  This software/database is freely available
 *  to the public for use. The National Library of Medicine and the U.S.
 *   Government have not placed any restriction on its use or reproduction.
 *
 *  Although all reasonable efforts have been taken to ensure the accuracy
 *  and reliability of the software and data, the NLM and the U.S.
 *  Government do not and cannot warrant the performance or results that
 *  may be obtained by using this software or data. The NLM and the U.S.
 *  Government disclaim all warranties, express or implied, including
 *  warranties of performance, merchantability or fitness for any particular
 *  purpose.
 *
 *  Please cite the author in any work or product based on this material.
 *
 * ===========================================================================
 *
 * Authors:  Maxim Didenko, Anatoliy Kuznetsov, Dmitry Kazimirov
 *
 * File Description:
 *    NetSchedule Worker Node implementation
 */

#include <ncbi_pch.hpp>

#include "wn_commit_thread.hpp"
#include "wn_cleanup.hpp"
#include "grid_worker_impl.hpp"
#include "netschedule_api_impl.hpp"

#include <connect/services/grid_globals.hpp>
#include <connect/services/grid_worker_app.hpp>
#include <connect/services/grid_rw_impl.hpp>
#include <connect/services/ns_job_serializer.hpp>

#include <corelib/rwstream.hpp>


#define NCBI_USE_ERRCODE_X   ConnServ_WorkerNode

BEGIN_NCBI_SCOPE

/////////////////////////////////////////////////////////////////////////////
//
//     CWorkerNodeJobContext     --


const CNetScheduleJob& CWorkerNodeJobContext::GetJob() const
{
    return m_Impl->m_Job;
}

CNetScheduleJob& CWorkerNodeJobContext::GetJob()
{
    return m_Impl->m_Job;
}

const string& CWorkerNodeJobContext::GetJobKey() const
{
    return m_Impl->m_Job.job_id;
}

const string& CWorkerNodeJobContext::GetJobInput() const
{
    return m_Impl->m_Job.input;
}

void CWorkerNodeJobContext::SetJobOutput(const string& output)
{
    m_Impl->m_Job.output = output;
}

void CWorkerNodeJobContext::SetJobRetCode(int ret_code)
{
    m_Impl->m_Job.ret_code = ret_code;
}

size_t CWorkerNodeJobContext::GetInputBlobSize() const
{
    return m_Impl->m_InputBlobSize;
}

const string& CWorkerNodeJobContext::GetJobOutput() const
{
    return m_Impl->m_Job.output;
}

CNetScheduleAPI::TJobMask CWorkerNodeJobContext::GetJobMask() const
{
    return m_Impl->m_Job.mask;
}

unsigned int CWorkerNodeJobContext::GetJobNumber() const
{
    return m_Impl->m_JobNumber;
}

bool CWorkerNodeJobContext::IsJobCommitted() const
{
    return m_Impl->m_JobCommitted != eNotCommitted;
}

CWorkerNodeJobContext::ECommitStatus
        CWorkerNodeJobContext::GetCommitStatus() const
{
    return m_Impl->m_JobCommitted;
}

bool CWorkerNodeJobContext::IsCanceled() const
{
    return m_Impl->m_JobCommitted == eCanceled;
}

IWorkerNodeCleanupEventSource* CWorkerNodeJobContext::GetCleanupEventSource()
{
    return m_Impl->m_CleanupEventSource;
}

CGridWorkerNode CWorkerNodeJobContext::GetWorkerNode() const
{
    return m_Impl->m_WorkerNode;
}

SWorkerNodeJobContextImpl::SWorkerNodeJobContextImpl(
        SGridWorkerNodeImpl* worker_node) :
    m_WorkerNode(worker_node),
    m_CleanupEventSource(
            new CWorkerNodeJobCleanup(worker_node->m_CleanupEventSource)),
    m_RequestContext(new CRequestContext),
    m_StatusThrottler(1, CTimeSpan(worker_node->m_CheckStatusPeriod, 0)),
    m_ProgressMsgThrottler(1),
    m_NetScheduleExecutor(worker_node->m_NSExecutor),
    m_NetCacheAPI(worker_node->m_NetCacheAPI),
    m_JobGeneration(worker_node->m_CurrentJobGeneration),
    m_CommitExpiration(0, 0)
{
}

const string& CWorkerNodeJobContext::GetQueueName() const
{
    return m_Impl->m_WorkerNode->GetQueueName();
}
const string& CWorkerNodeJobContext::GetClientName() const
{
    return m_Impl->m_WorkerNode->GetClientName();
}

CNcbiIstream& CWorkerNodeJobContext::GetIStream()
{
    IReader* reader = new CStringOrBlobStorageReader(GetJobInput(),
            m_Impl->m_NetCacheAPI, &m_Impl->m_InputBlobSize);
    m_Impl->m_RStream.reset(new CRStream(reader, 0, 0,
            CRWStreambuf::fOwnReader | CRWStreambuf::fLeakExceptions));
    m_Impl->m_RStream->exceptions(IOS_BASE::badbit | IOS_BASE::failbit);
    return *m_Impl->m_RStream;
}
CNcbiOstream& CWorkerNodeJobContext::GetOStream()
{
    m_Impl->m_Writer.reset(new CStringOrBlobStorageWriter(
            m_Impl->m_WorkerNode->m_QueueEmbeddedOutputSize,
                    m_Impl->m_NetCacheAPI, GetJob().output));
    m_Impl->m_WStream.reset(new CWStream(m_Impl->m_Writer.get(), 0, 0,
            CRWStreambuf::fLeakExceptions));
    m_Impl->m_WStream->exceptions(IOS_BASE::badbit | IOS_BASE::failbit);
    return *m_Impl->m_WStream;
}

void CWorkerNodeJobContext::CloseStreams()
{
    try {
        m_Impl->m_ProgressMsgThrottler.Reset(1);
        m_Impl->m_StatusThrottler.Reset(1,
                CTimeSpan(m_Impl->m_WorkerNode->m_CheckStatusPeriod, 0));

        m_Impl->m_RStream.reset();
        m_Impl->m_WStream.reset();

        if (m_Impl->m_Writer.get() != NULL) {
            m_Impl->m_Writer->Close();
            m_Impl->m_Writer.reset();
        }
    }
    NCBI_CATCH_ALL_X(61, "Could not close IO streams");
}

void CWorkerNodeJobContext::CommitJob()
{
    m_Impl->CheckIfCanceled();
    m_Impl->m_JobCommitted = eDone;
}

void CWorkerNodeJobContext::CommitJobWithFailure(const string& err_msg,
        bool no_retries)
{
    m_Impl->CheckIfCanceled();
    m_Impl->m_JobCommitted = eFailure;
    m_Impl->m_DisableRetries = no_retries;
    m_Impl->m_Job.error_msg = err_msg;
}

void CWorkerNodeJobContext::ReturnJob()
{
    m_Impl->CheckIfCanceled();
    m_Impl->m_JobCommitted = eReturn;
}

void CWorkerNodeJobContext::RescheduleJob(
        const string& affinity, const string& group)
{
    m_Impl->CheckIfCanceled();
    m_Impl->m_JobCommitted = eRescheduled;
    m_Impl->m_Job.affinity = affinity;
    m_Impl->m_Job.group = group;
}

void CWorkerNodeJobContext::PutProgressMessage(const string& msg,
                                               bool send_immediately)
{
    m_Impl->CheckIfCanceled();
    if (!send_immediately &&
            !m_Impl->m_ProgressMsgThrottler.Approve(
                    CRequestRateControl::eErrCode)) {
        LOG_POST(Warning << "Progress message \"" <<
                msg << "\" has been suppressed.");
        return;
    }

    if (m_Impl->m_WorkerNode->m_ProgressLogRequested) {
        LOG_POST(GetJobKey() << " progress: " <<
                NStr::TruncateSpaces(msg, NStr::eTrunc_End));
    }

    try {
        if (m_Impl->m_Job.progress_msg.empty() ) {
            m_Impl->m_NetScheduleExecutor.GetProgressMsg(m_Impl->m_Job);
        }
        if (!m_Impl->m_Job.progress_msg.empty())
            m_Impl->m_NetCacheAPI.PutData(m_Impl->m_Job.progress_msg,
                    msg.data(), msg.length());
        else {
            m_Impl->m_Job.progress_msg =
                    m_Impl->m_NetCacheAPI.PutData(msg.data(), msg.length());

            m_Impl->m_NetScheduleExecutor.PutProgressMsg(m_Impl->m_Job);
        }
    }
    catch (exception& ex) {
        ERR_POST_X(6, "Couldn't send a progress message: " << ex.what());
    }
}

void CWorkerNodeJobContext::JobDelayExpiration(unsigned runtime_inc)
{
    try {
        m_Impl->m_NetScheduleExecutor.JobDelayExpiration(GetJobKey(),
                runtime_inc);
    }
    catch (exception& ex) {
        ERR_POST_X(8, "CWorkerNodeJobContext::JobDelayExpiration: " <<
                ex.what());
    }
}

bool CWorkerNodeJobContext::IsLogRequested() const
{
    return m_Impl->m_WorkerNode->m_LogRequested;
}

CNetScheduleAdmin::EShutdownLevel CWorkerNodeJobContext::GetShutdownLevel()
{
    if (m_Impl->m_StatusThrottler.Approve(CRequestRateControl::eErrCode))
        try {
            ENetScheduleQueuePauseMode pause_mode;
            switch (m_Impl->m_NetScheduleExecutor.GetJobStatus(GetJobKey(),
                    NULL, &pause_mode)) {
            case CNetScheduleAPI::eRunning:
                if (pause_mode == eNSQ_WithPullback) {
                    m_Impl->m_WorkerNode->SetJobPullbackTimer(
                            m_Impl->m_WorkerNode->m_DefaultPullbackTimeout);
                    LOG_POST("Pullback request from the server, "
                            "(default) pullback timeout=" <<
                            m_Impl->m_WorkerNode->m_DefaultPullbackTimeout);
                }
                break;

            case CNetScheduleAPI::eCanceled:
                m_Impl->x_SetCanceled();
                /* FALL THROUGH */

            default:
                return CNetScheduleAdmin::eShutdownImmediate;
            }
        }
        catch(exception& ex) {
            ERR_POST("Cannot retrieve job status for " << GetJobKey() <<
                    ": " << ex.what());
        }

    if (m_Impl->m_WorkerNode->CheckForPullback(m_Impl->m_JobGeneration)) {
        LOG_POST("Pullback timeout for " << m_Impl->m_Job.job_id);
        return CNetScheduleAdmin::eShutdownImmediate;
    }

    return CGridGlobals::GetInstance().GetShutdownLevel();
}

void SWorkerNodeJobContextImpl::CheckIfCanceled()
{
    if (m_JobCommitted == CWorkerNodeJobContext::eCanceled) {
        NCBI_THROW_FMT(CGridWorkerNodeException, eJobIsCanceled,
            "Job " << m_Job.job_id << " has been canceled");
    }
}

void SWorkerNodeJobContextImpl::ResetJobContext()
{
    m_JobNumber = CGridGlobals::GetInstance().GetNewJobNumber();

    m_JobCommitted = CWorkerNodeJobContext::eNotCommitted;
    m_DisableRetries = false;
    m_InputBlobSize = 0;
    m_ExclusiveJob =
            (m_Job.mask & CNetScheduleAPI::eExclusiveJob) != 0;

    m_RequestContext->Reset();
}

void CWorkerNodeJobContext::RequestExclusiveMode()
{
    if (!m_Impl->m_ExclusiveJob) {
        if (!m_Impl->m_WorkerNode->EnterExclusiveMode()) {
            NCBI_THROW(CGridWorkerNodeException,
                eExclusiveModeIsAlreadySet, "");
        }
        m_Impl->m_ExclusiveJob = true;
    }
}

const char* CWorkerNodeJobContext::GetCommitStatusDescription(
        CWorkerNodeJobContext::ECommitStatus commit_status)
{
    switch (commit_status) {
    case eDone:
        return "done";
    case eFailure:
        return "failed";
    case eReturn:
        return "returned";
    case eRescheduled:
        return "rescheduled";
    case eCanceled:
        return "canceled";
    default:
        return "not committed";
    }
}

void SWorkerNodeJobContextImpl::x_PrintRequestStop()
{
    m_RequestContext->SetAppState(eDiagAppState_RequestEnd);

    if (!m_RequestContext->IsSetRequestStatus())
        m_RequestContext->SetRequestStatus(
            m_JobCommitted == CWorkerNodeJobContext::eDone &&
                m_Job.ret_code == 0 ? 200 : 500);

    if (m_RequestContext->GetAppState() == eDiagAppState_Request)
        m_RequestContext->SetAppState(eDiagAppState_RequestEnd);

    if (g_IsRequestStopEventEnabled())
        GetDiagContext().PrintRequestStop();
}

void SWorkerNodeJobContextImpl::x_RunJob()
{
    CWorkerNodeJobContext this_job_context(this);

    m_RequestContext->SetRequestID((int) this_job_context.GetJobNumber());

    if (!m_Job.client_ip.empty())
        m_RequestContext->SetClientIP(m_Job.client_ip);

    if (!m_Job.session_id.empty())
        m_RequestContext->SetSessionID(m_Job.session_id);

    m_RequestContext->SetAppState(eDiagAppState_RequestBegin);

    CRequestContextSwitcher request_state_guard(m_RequestContext);

    if (g_IsRequestStartEventEnabled())
        GetDiagContext().PrintRequestStart().Print("jid", m_Job.job_id);

    m_RequestContext->SetAppState(eDiagAppState_Request);

    CJobRunRegistration client_ip_registration, session_id_registration;

    if (!m_Job.client_ip.empty() &&
            !m_WorkerNode->m_JobsPerClientIP.CountJob(m_Job.client_ip,
                    &client_ip_registration)) {
        ERR_POST("Too many jobs with client IP \"" <<
                 m_Job.client_ip << "\"; job " <<
                 m_Job.job_id << " will be returned.");
        m_JobCommitted = CWorkerNodeJobContext::eReturn;
    } else if (!m_Job.session_id.empty() &&
            !m_WorkerNode->m_JobsPerSessionID.CountJob(m_Job.session_id,
                    &session_id_registration)) {
        ERR_POST("Too many jobs with session ID \"" <<
                 m_Job.session_id << "\"; job " <<
                 m_Job.job_id << " will be returned.");
        m_JobCommitted = CWorkerNodeJobContext::eReturn;
    } else {
        m_WorkerNode->x_NotifyJobWatchers(this_job_context,
                IWorkerNodeJobWatcher::eJobStarted);

        try {
            m_Job.ret_code =
                    m_WorkerNode->GetJobProcessor()->Do(this_job_context);
        }
        catch (CGridWorkerNodeException& ex) {
            switch (ex.GetErrCode()) {
            case CGridWorkerNodeException::eJobIsCanceled:
                m_JobCommitted = CWorkerNodeJobContext::eCanceled;
                break;

            case CGridWorkerNodeException::eExclusiveModeIsAlreadySet:
                if (m_WorkerNode->m_LogRequested) {
                    LOG_POST_X(21, "Job " << m_Job.job_id <<
                        " will be returned back to the queue "
                        "because it requested exclusive mode while "
                        "another exclusive job is already running.");
                }
                if (m_JobCommitted == CWorkerNodeJobContext::eNotCommitted)
                    m_JobCommitted = CWorkerNodeJobContext::eReturn;
                break;

            default:
                ERR_POST_X(62, ex);
                if (m_JobCommitted == CWorkerNodeJobContext::eNotCommitted)
                    m_JobCommitted = CWorkerNodeJobContext::eReturn;
            }
        }
        catch (CNetScheduleException& e) {
            ERR_POST_X(20, "job " << m_Job.job_id << " failed: " << e);
            if (e.GetErrCode() == CNetScheduleException::eJobNotFound)
                m_JobCommitted = CWorkerNodeJobContext::eCanceled;
            else if (m_JobCommitted == CWorkerNodeJobContext::eNotCommitted) {
                m_JobCommitted = CWorkerNodeJobContext::eFailure;
                m_Job.error_msg = e.what();
            }
        }
        catch (exception& e) {
            ERR_POST_X(18, "job " << m_Job.job_id << " failed: " << e.what());
            if (m_JobCommitted == CWorkerNodeJobContext::eNotCommitted) {
                m_JobCommitted = CWorkerNodeJobContext::eFailure;
                m_Job.error_msg = e.what();
            }
        }

        this_job_context.CloseStreams();

        switch (m_JobCommitted) {
        case CWorkerNodeJobContext::eDone:
            m_WorkerNode->x_NotifyJobWatchers(this_job_context,
                    IWorkerNodeJobWatcher::eJobSucceeded);
            break;

        case CWorkerNodeJobContext::eNotCommitted:
            if (TWorkerNode_AllowImplicitJobReturn::GetDefault() ||
                    this_job_context.GetShutdownLevel() !=
                            CNetScheduleAdmin::eNoShutdown) {
                m_JobCommitted = CWorkerNodeJobContext::eReturn;
                m_WorkerNode->x_NotifyJobWatchers(this_job_context,
                        IWorkerNodeJobWatcher::eJobReturned);
                break;
            }

            m_JobCommitted = CWorkerNodeJobContext::eFailure;
            m_Job.error_msg = "Job was not explicitly committed";
            /* FALL THROUGH */

        case CWorkerNodeJobContext::eFailure:
            m_WorkerNode->x_NotifyJobWatchers(this_job_context,
                    IWorkerNodeJobWatcher::eJobFailed);
            break;

        case CWorkerNodeJobContext::eReturn:
            m_WorkerNode->x_NotifyJobWatchers(this_job_context,
                    IWorkerNodeJobWatcher::eJobReturned);
            break;

        case CWorkerNodeJobContext::eRescheduled:
            m_WorkerNode->x_NotifyJobWatchers(this_job_context,
                    IWorkerNodeJobWatcher::eJobRescheduled);
            break;

        default: // eCanceled - will be processed in x_SendJobResults().
            break;
        }

        m_WorkerNode->x_NotifyJobWatchers(this_job_context,
                IWorkerNodeJobWatcher::eJobStopped);
    }

    if (m_WorkerNode->IsExclusiveMode() && m_ExclusiveJob)
        m_WorkerNode->LeaveExclusiveMode();

    if (!CGridGlobals::GetInstance().IsShuttingDown())
        m_CleanupEventSource->CallEventHandlers();

    m_WorkerNode->m_JobCommitterThread->RecycleJobContextAndCommitJob(this);
}

void* CMainLoopThread::Main()
{
    CDeadline max_wait_for_servers(
            TWorkerNode_MaxWaitForServers::GetDefault());

    CWorkerNodeJobContext job_context(
            m_WorkerNode->m_JobCommitterThread->AllocJobContext());

    unsigned try_count = 0;
    while (!CGridGlobals::GetInstance().IsShuttingDown()) {
        try {
            try {
                m_WorkerNode->m_ThreadPool->WaitForRoom(
                        m_WorkerNode->m_ThreadPoolTimeout);
            }
            catch (CBlockingQueueException&) {
                // threaded pool is busy
                continue;
            }

            if (x_GetNextJob(job_context->m_Job)) {
                job_context->ResetJobContext();

                try {
                    m_WorkerNode->m_ThreadPool->AcceptRequest(CRef<CStdRequest>(
                            new CWorkerNodeRequest(job_context)));
                }
                catch (CBlockingQueueException& ex) {
                    ERR_POST_X(28, ex);
                    // that must not happen after CBlockingQueue is fixed
                    _ASSERT(0);
                    job_context->m_JobCommitted =
                            CWorkerNodeJobContext::eReturn;
                    m_WorkerNode->m_JobCommitterThread->
                            RecycleJobContextAndCommitJob(job_context);
                }
                job_context =
                        m_WorkerNode->m_JobCommitterThread->AllocJobContext();
            }
            max_wait_for_servers =
                CDeadline(TWorkerNode_MaxWaitForServers::GetDefault());
        }
        catch (CNetSrvConnException& e) {
            SleepMilliSec(s_GetRetryDelay());
            if (e.GetErrCode() == CNetSrvConnException::eConnectionFailure &&
                    !max_wait_for_servers.GetRemainingTime().IsZero())
                continue;
            ERR_POST(Critical << "Could not connect to the "
                    "configured servers, exiting...");
            CGridGlobals::GetInstance().RequestShutdown(
                    CNetScheduleAdmin::eShutdownImmediate);
        }
        catch (CNetServiceException& ex) {
            ERR_POST_X(40, ex);
            if (++try_count >= TServConn_ConnMaxRetries::GetDefault()) {
                CGridGlobals::GetInstance().RequestShutdown(
                    CNetScheduleAdmin::eShutdownImmediate);
            } else {
                SleepMilliSec(s_GetRetryDelay());
                continue;
            }
        }
        catch (exception& ex) {
            ERR_POST_X(29, ex.what());
            if (TWorkerNode_StopOnJobErrors::GetDefault()) {
                CGridGlobals::GetInstance().RequestShutdown(
                    CNetScheduleAdmin::eShutdownImmediate);
            }
        }
        try_count = 0;
    }

    return NULL;
}

CMainLoopThread::~CMainLoopThread()
{
    ITERATE(TTimelineEntries, it, m_TimelineEntryByAddress) {
        (*it)->RemoveReference();
    }
}

bool CMainLoopThread::x_GetJobWithAffinityList(SNetServerImpl* server,
        const CDeadline* timeout, CNetScheduleJob& job,
        CNetScheduleExecutor::EJobAffinityPreference affinity_preference,
        const string& affinity_list)
{
    string cmd(m_WorkerNode->m_NSExecutor->
            m_NotificationHandler.CmdAppendTimeoutAndClientInfo(
                    CNetScheduleNotificationHandler::MkBaseGETCmd(
                            affinity_preference, affinity_list), timeout));

    return m_WorkerNode->m_NSExecutor->ExecGET(server, cmd, job);
}

bool CMainLoopThread::x_GetJobWithAffinityLadder(SNetServerImpl* server,
        const CDeadline* timeout, CNetScheduleJob& job)
{
    if (m_WorkerNode->m_NSExecutor->m_API->m_AffinityLadder.empty())
        return x_GetJobWithAffinityList(server, timeout, job,
                m_WorkerNode->m_NSExecutor->m_AffinityPreference, kEmptyStr);

    list<string>::const_iterator it =
            m_WorkerNode->m_NSExecutor->m_API->m_AffinityLadder.begin();

    for (;;) {
        string affinity_list = *it;
        if (++it == m_WorkerNode->m_NSExecutor->m_API->m_AffinityLadder.end())
            return x_GetJobWithAffinityList(server, timeout, job,
                    m_WorkerNode->m_NSExecutor->m_AffinityPreference,
                            affinity_list);
        else if (x_GetJobWithAffinityList(server, NULL, job,
                CNetScheduleExecutor::ePreferredAffinities, affinity_list))
            return true;
    }
}

SNotificationTimelineEntry*
    CMainLoopThread::x_GetTimelineEntry(SNetServerImpl* server_impl)
{
    SNotificationTimelineEntry search_pattern(
            server_impl->m_ServerInPool->m_Address, 0);

    TTimelineEntries::iterator it(
            m_TimelineEntryByAddress.find(&search_pattern));

    if (it != m_TimelineEntryByAddress.end())
        return *it;

    SNotificationTimelineEntry* new_entry = new SNotificationTimelineEntry(
            search_pattern.m_ServerAddress, m_DiscoveryIteration);

    m_TimelineEntryByAddress.insert(new_entry);
    new_entry->AddReference();

    return new_entry;
}

bool CMainLoopThread::x_PerformTimelineAction(
        CMainLoopThread::TNotificationTimeline& timeline, CNetScheduleJob& job)
{
    SNotificationTimelineEntry::TRef timeline_entry;

    timeline.Shift(timeline_entry);

    if (timeline_entry->IsDiscoveryAction()) {
        if (!x_EnterSuspendedState()) {
            ++m_DiscoveryIteration;
            for (CNetServiceIterator it =
                    m_WorkerNode->m_NetScheduleAPI.GetService().Iterate(
                            CNetService::eIncludePenalized); it; ++it) {
                SNotificationTimelineEntry* srv_entry = x_GetTimelineEntry(*it);
                srv_entry->m_DiscoveryIteration = m_DiscoveryIteration;
                if (!srv_entry->IsInTimeline())
                    m_ImmediateActions.Push(srv_entry);
            }
        }

        timeline_entry->ResetTimeout(m_WorkerNode->m_NSTimeout);
        m_Timeline.Push(timeline_entry);
        return false;
    }

    if (x_EnterSuspendedState() ||
            // Skip servers that disappeared from LBSM.
            timeline_entry->m_DiscoveryIteration != m_DiscoveryIteration)
        return false;

    CNetServer server(m_WorkerNode->m_NetScheduleAPI->m_Service.GetServer(
            timeline_entry->m_ServerAddress));

    timeline_entry->ResetTimeout(m_WorkerNode->m_NSTimeout);

    try {
        if (x_GetJobWithAffinityLadder(server,
                &timeline_entry->GetTimeout(), job)) {
            // A job has been returned; add the server to
            // m_ImmediateActions because there can be more
            // jobs in the queue.
            m_ImmediateActions.Push(timeline_entry);
            return true;
        } else {
            // No job has been returned by this server;
            // query the server later.
            m_Timeline.Push(timeline_entry);
            return false;
        }
    }
    catch (CNetSrvConnException& e) {
        // Because a connection error has occurred, do not
        // put this server back to the timeline.
        LOG_POST(Warning << e.GetMsg());
        return false;
    }
}

bool CMainLoopThread::x_EnterSuspendedState()
{
    if (CGridGlobals::GetInstance().IsShuttingDown())
        return true;

    void* event;

    while ((event = SwapPointers(&m_WorkerNode->m_SuspendResumeEvent,
            NO_EVENT)) != NO_EVENT) {
        if (event == SUSPEND_EVENT) {
            if (!m_WorkerNode->m_TimelineIsSuspended) {
                // Stop the timeline.
                m_WorkerNode->m_TimelineIsSuspended = true;
                m_ImmediateActions.Clear();
                m_Timeline.Clear();
                m_DiscoveryAction->ResetTimeout(m_WorkerNode->m_NSTimeout);
                m_Timeline.Push(m_DiscoveryAction);
            }
        } else { /* event == RESUME_EVENT */
            if (m_WorkerNode->m_TimelineIsSuspended) {
                // Resume the timeline.
                m_WorkerNode->m_TimelineIsSuspended = false;
                m_DiscoveryAction->MoveTo(&m_ImmediateActions);
            }
        }
    }

    return m_WorkerNode->m_TimelineIsSuspended;
}

void CMainLoopThread::x_ProcessRequestJobNotification()
{
    if (!x_EnterSuspendedState()) {
        CNetServer server;

        if (m_WorkerNode->m_NSExecutor->
                m_NotificationHandler.CheckRequestJobNotification(
                        m_WorkerNode->m_NSExecutor, &server))
            x_GetTimelineEntry(server)->MoveTo(&m_ImmediateActions);
    }
}

bool CMainLoopThread::x_WaitForNewJob(CNetScheduleJob& job)
{
    for (;;) {
        while (!m_ImmediateActions.IsEmpty()) {
            if (x_PerformTimelineAction(m_ImmediateActions, job))
                return true;

            while (!m_Timeline.IsEmpty() && m_Timeline.GetHead()->TimeHasCome())
                m_Timeline.MoveHeadTo(&m_ImmediateActions);

            // Check if there's a notification in the UDP socket.
            while (m_WorkerNode->m_NSExecutor->
                    m_NotificationHandler.ReceiveNotification())
                x_ProcessRequestJobNotification();
        }

        if (CGridGlobals::GetInstance().IsShuttingDown())
            return false;

        if (!m_Timeline.IsEmpty()) {
            if (m_WorkerNode->m_NSExecutor->
                    m_NotificationHandler.WaitForNotification(
                            m_Timeline.GetHead()->GetTimeout()))
                x_ProcessRequestJobNotification();
            else if (x_PerformTimelineAction(m_Timeline, job))
                return true;
        }
    }
}

bool CMainLoopThread::x_GetNextJob(CNetScheduleJob& job)
{
    if (!m_WorkerNode->x_AreMastersBusy()) {
        SleepSec(m_WorkerNode->m_NSTimeout);
        return false;
    }

    if (!m_WorkerNode->WaitForExclusiveJobToFinish())
        return false;

    bool job_exists = x_WaitForNewJob(job);

    if (job_exists && job.mask & CNetScheduleAPI::eExclusiveJob) {
        if (!m_WorkerNode->EnterExclusiveMode()) {
            x_ReturnJob(job);
            job_exists = false;
        }
    }
    if (job_exists && x_EnterSuspendedState()) {
        x_ReturnJob(job);
        return false;
    }
    return job_exists;
}

void CMainLoopThread::x_ReturnJob(const CNetScheduleJob& job)
{
    m_WorkerNode->m_NSExecutor.ReturnJob(job.job_id, job.auth_token);
}

size_t CGridWorkerNode::GetServerOutputSize()
{
    return m_Impl->m_QueueEmbeddedOutputSize;
}

END_NCBI_SCOPE
