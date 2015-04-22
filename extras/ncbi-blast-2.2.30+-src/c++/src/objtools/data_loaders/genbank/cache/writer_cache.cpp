/*  $Id: writer_cache.cpp 447937 2014-10-01 17:41:13Z vasilche $
 * ===========================================================================
 *                            PUBLIC DOMAIN NOTICE
 *               National Center for Biotechnology Information
 *
 *  This software/database is a "United States Government Work" under the
 *  terms of the United States Copyright Act.  It was written as part of
 *  the author's official duties as a United States Government employee and
 *  thus cannot be copyrighted.  This software/database is freely available
 *  to the public for use. The National Library of Medicine and the U.S.
 *  Government have not placed any restriction on its use or reproduction.
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
 *  Author:  Eugene Vasilchenko, Anatoliy Kuznetsov
 *
 *  File Description: Cached writer for GenBank data loader
 *
 */
#include <ncbi_pch.hpp>
#include <objtools/data_loaders/genbank/cache/writer_cache.hpp>
#include <objtools/data_loaders/genbank/cache/writer_cache_entry.hpp>
#include <objtools/data_loaders/genbank/cache/reader_cache_params.h>
#include <objtools/data_loaders/genbank/readers.hpp> // for entry point
#include <objtools/data_loaders/genbank/impl/request_result.hpp>
#include <objtools/data_loaders/genbank/impl/dispatcher.hpp>

#include <corelib/rwstream.hpp>
#include <corelib/plugin_manager_store.hpp>

#include <serial/objostrasnb.hpp>
#include <serial/serial.hpp>

#include <objmgr/objmgr_exception.hpp>

#include <util/cache/icache.hpp>

#include <memory>

BEGIN_NCBI_SCOPE
BEGIN_SCOPE(objects)

CCacheWriter::CCacheWriter(void)
{
}


void CCacheWriter::InitializeCache(CReaderCacheManager& cache_manager,
                                   const TPluginManagerParamTree* params)
{
    const TPluginManagerParamTree* writer_params = params ?
        params->FindNode(NCBI_GBLOADER_WRITER_CACHE_DRIVER_NAME) : 0;
    ICache* id_cache = 0;
    ICache* blob_cache = 0;
    auto_ptr<TParams> id_params
        (GetCacheParams(writer_params, eCacheWriter, eIdCache));
    auto_ptr<TParams> blob_params
        (GetCacheParams(writer_params, eCacheWriter, eBlobCache));
    _ASSERT(id_params.get());
    _ASSERT(blob_params.get());
    const TParams* share_id_param =
        id_params->FindNode(NCBI_GBLOADER_WRITER_CACHE_PARAM_SHARE);
    bool share_id = !share_id_param  ||
        NStr::StringToBool(share_id_param->GetValue().value);
    const TParams* share_blob_param =
        blob_params->FindNode(NCBI_GBLOADER_WRITER_CACHE_PARAM_SHARE);
    bool share_blob = !share_blob_param  ||
        NStr::StringToBool(share_blob_param->GetValue().value);
    if (share_id  ||  share_blob) {
        if ( share_id ) {
            ICache* cache = cache_manager.
                FindCache(CReaderCacheManager::fCache_Id,
                          id_params.get());
            if ( cache ) {
                _ASSERT(!id_cache);
                id_cache = cache;
            }
        }
        if ( share_blob ) {
            ICache* cache = cache_manager.
                FindCache(CReaderCacheManager::fCache_Blob,
                          blob_params.get());
            if ( cache ) {
                _ASSERT(!blob_cache);
                blob_cache = cache;
            }
        }
    }
    if ( !id_cache ) {
        id_cache = CreateCache(writer_params, eCacheWriter, eIdCache);
        if ( id_cache ) {
            cache_manager.RegisterCache(*id_cache,
                CReaderCacheManager::fCache_Id);
        }
    }
    if ( !blob_cache ) {
        blob_cache = CreateCache(writer_params, eCacheWriter, eBlobCache);
        if ( blob_cache ) {
            cache_manager.RegisterCache(*blob_cache,
                CReaderCacheManager::fCache_Blob);
        }
    }
    SetIdCache(id_cache);
    SetBlobCache(blob_cache);
}


void CCacheWriter::ResetCache(void)
{
    SetIdCache(0);
    SetBlobCache(0);
}


void CCacheWriter::SaveStringSeq_ids(CReaderRequestResult& result,
                                     const string& seq_id)
{
    if ( !m_IdCache) {
        return;
    }

    CLoadLockSeqIds ids(result, seq_id);
    WriteSeq_ids(seq_id, ids);
}


void CCacheWriter::CStoreBuffer::CheckSpace(size_t add)
{
    if ( m_Ptr + add > m_End ) {
        size_t old_size = size();
        size_t new_size = (old_size+add)*2;
        char* new_buf = new char[new_size];
        memcpy(new_buf, data(), old_size);
        x_FreeBuffer();
        m_Buffer = new_buf;
        m_Ptr = new_buf + old_size;
        m_End = new_buf + new_size;
    }
    _ASSERT(m_Ptr + add <= m_End);
}


void CCacheWriter::CStoreBuffer::StoreString(const string& s)
{
    size_t size = s.size();
    CheckSpace(4+size);
    x_StoreUint4(ToUint4(size));
    memcpy(m_Ptr, s.data(), size);
    m_Ptr += size;
}


void CCacheWriter::CStoreBuffer::x_FreeBuffer(void)
{
    if ( m_Buffer != m_Buffer0 ) {
        delete[] m_Buffer;
        m_Buffer = m_End = m_Ptr = 0;
    }
}


void CCacheWriter::x_WriteId(const string& key,
                             const string& subkey,
                             const char* data,
                             size_t size)
{
    try {
        if ( GetDebugLevel() > 0 ) {
            CReader::CDebugPrinter s("CCacheWriter");
            s<<key<<","<<subkey<<" size="<<size;
        }
        m_IdCache->Store(key, 0, subkey, data, size);
    }
    catch ( exception& ) { // ignored
    }
}


void CCacheWriter::SaveStringGi(CReaderRequestResult& result,
                                const string& seq_id)
{
    if( !m_IdCache) {
        return;
    }

    CLoadLockGi lock(result, seq_id);
    if ( lock.IsLoadedGi() ) {
        CStoreBuffer str;
        str.StoreInt4(GI_TO(Int4, lock.GetGi()));
        x_WriteId(seq_id, GetGiSubkey(), str);
    }
}


void CCacheWriter::SaveSeq_idSeq_ids(CReaderRequestResult& result,
                                     const CSeq_id_Handle& seq_id)
{
    if( !m_IdCache) {
        return;
    }

    CLoadLockSeqIds ids(result, seq_id);
    WriteSeq_ids(GetIdKey(seq_id), ids);
}


void CCacheWriter::SaveSeq_idGi(CReaderRequestResult& result,
                                const CSeq_id_Handle& seq_id)
{
    if( !m_IdCache) {
        return;
    }

    CLoadLockGi lock(result, seq_id);
    if ( lock.IsLoadedGi() ) {
        CStoreBuffer str;
        str.StoreInt4(GI_TO(Int4, lock.GetGi()));
        x_WriteId(GetIdKey(seq_id), GetGiSubkey(), str);
    }
}


void CCacheWriter::SaveSeq_idAccVer(CReaderRequestResult& result,
                                    const CSeq_id_Handle& seq_id)
{
    if( !m_IdCache) {
        return;
    }

    CLoadLockAcc lock(result, seq_id);
    if ( lock.IsLoadedAccVer() ) {
        string str;
        if ( CSeq_id_Handle acc = lock.GetAccVer() ) {
            str = acc.AsString();
        }
        x_WriteId(GetIdKey(seq_id), GetAccVerSubkey(), str);
    }
}


void CCacheWriter::SaveSeq_idLabel(CReaderRequestResult& result,
                                   const CSeq_id_Handle& seq_id)
{
    if( !m_IdCache) {
        return;
    }

    CLoadLockLabel lock(result, seq_id);
    if ( lock.IsLoadedLabel() ) {
        const string& str = lock.GetLabel();
        x_WriteId(GetIdKey(seq_id), GetLabelSubkey(), str);
    }
}


void CCacheWriter::SaveSeq_idTaxId(CReaderRequestResult& result,
                                   const CSeq_id_Handle& seq_id)
{
    if( !m_IdCache) {
        return;
    }

    CLoadLockTaxId lock(result, seq_id);
    if ( lock.IsLoadedTaxId() ) {
        CStoreBuffer str;
        str.StoreInt4(lock.GetTaxId());
        x_WriteId(GetIdKey(seq_id), GetTaxIdSubkey(), str);
    }
}


void CCacheWriter::WriteSeq_ids(const string& key,
                                const CLoadLockSeqIds& lock)
{
    if( !m_IdCache) {
        return;
    }

    if ( !lock.IsLoaded() ) {
        return;
    }

    try {
        if ( GetDebugLevel() > 0 ) {
            CReader::CDebugPrinter s("CCacheWriter");
            s<<key<<","<<GetSeq_idsSubkey();
        }
        auto_ptr<IWriter> writer
            (m_IdCache->GetWriteStream(key, 0, GetSeq_idsSubkey()));
        if ( !writer.get() ) {
            return;
        }

        CWStream w_stream(writer.release(), 0, 0, CRWStreambuf::fOwnAll);
        CObjectOStreamAsnBinary obj_stream(w_stream);
        CFixedSeq_ids seq_ids = lock.GetSeq_ids();
        static_cast<CObjectOStream&>(obj_stream). // cast because of protected
            WriteUint4(CStoreBuffer::ToUint4(seq_ids.size()));
        ITERATE ( CFixedSeq_ids, it, seq_ids ) {
            obj_stream << *it->GetSeqId();
        }
    }
    catch ( exception& ) {
        // In case of an error we need to remove incomplete data
        // from the cache.
        try {
            m_IdCache->Remove(key, 0, GetSeq_idsSubkey());
        }
        catch ( exception& ) { // ignored
        }
        // ignore cache write error - it doesn't affect application
    }
}


void CCacheWriter::SaveSeq_idBlob_ids(CReaderRequestResult& result,
                                      const CSeq_id_Handle& seq_id,
                                      const SAnnotSelector* sel)
{
    if ( !m_IdCache ) {
        return;
    }

    CLoadLockBlobIds ids(result, seq_id, sel);
    if ( !ids.IsLoaded() ) {
        return;
    }

    string subkey, true_subkey;
    GetBlob_idsSubkey(sel, subkey, true_subkey);
    CStoreBuffer str;
    str.StoreInt4(BLOB_IDS_MAGIC);
    CFixedBlob_ids blob_ids = ids.GetBlob_ids();
    str.StoreUint4(blob_ids.GetState());
    str.StoreUint4(str.ToUint4(blob_ids.size()));
    ITERATE ( CFixedBlob_ids, it, blob_ids ) {
        const CBlob_Info& info = *it;
        const CBlob_id& id = *info.GetBlob_id();
        str.StoreUint4(id.GetSat());
        str.StoreUint4(id.GetSubSat());
        str.StoreUint4(id.GetSatKey());
        str.StoreUint4(info.GetContentsMask());
        CConstRef<CBlob_Annot_Info> annot_info = info.GetAnnotInfo();
        if ( annot_info ) {
            const CBlob_Annot_Info::TNamedAnnotNames& names =
                annot_info->GetNamedAnnotNames();
            str.StoreUint4(str.ToUint4(names.size()));
            ITERATE ( CBlob_Annot_Info::TNamedAnnotNames, it2, names ) {
                str.StoreString(*it2);
            }
        }
        else {
            str.StoreUint4(0);
        }
        if ( !annot_info || annot_info->GetAnnotInfo().empty() ) {
            str.StoreString(kEmptyStr);
        }
        else {
            CNcbiOstrstream stream_str;
            {{
                CObjectOStreamAsnBinary stream(stream_str);
                ITERATE( CBlob_Annot_Info::TAnnotInfo, it2,
                         annot_info->GetAnnotInfo() ) {
                    stream << **it2;
                }
            }}
            str.StoreString(CNcbiOstrstreamToString(stream_str));
        }
    }
    if ( !true_subkey.empty() ) {
        str.StoreString(true_subkey);
    }
    x_WriteId(GetIdKey(seq_id), subkey, str);
}


void CCacheWriter::SaveBlobState(CReaderRequestResult& /*result*/,
                                 const TBlobId& blob_id,
                                 TBlobState blob_state)
{
    if( !m_IdCache ) {
        return;
    }

    _ASSERT(blob_state >= 0);
    CStoreBuffer str;
    str.StoreInt4(blob_state);
    x_WriteId(GetBlobKey(blob_id), GetBlobStateSubkey(), str);
}


void CCacheWriter::SaveBlobVersion(CReaderRequestResult& /*result*/,
                                   const TBlobId& blob_id,
                                   TBlobVersion version)
{
    if( !m_IdCache ) {
        return;
    }

    _ASSERT(version >= 0);
    CStoreBuffer str;
    str.StoreInt4(version);
    x_WriteId(GetBlobKey(blob_id), GetBlobVersionSubkey(), str);
}


class CCacheBlobStream : public CWriter::CBlobStream
{
public:
    typedef int TVersion;

    CCacheBlobStream(ICache* cache, const string& key,
                     TVersion version, const string& subkey)
        : m_Cache(cache), m_Key(key), m_Version(version), m_Subkey(subkey),
          m_Writer(cache->GetWriteStream(key, version, subkey))
        {
            _ASSERT(version >= 0);
            if ( SCacheInfo::GetDebugLevel() > 0 ) {
                CReader::CDebugPrinter s("CCacheWriter");
                s<<key<<","<<subkey<<","<<version;
            }
            if ( version == -1 ) {
                ERR_POST("Cache:Write: "<<key<<","<<subkey<<","<<version);
            }
            if ( m_Writer.get() ) {
                m_Stream.reset(new CWStream(m_Writer.get()));
            }
        }
    ~CCacheBlobStream(void)
        {
            if ( m_Stream.get() ) {
                Abort();
            }
        }

    bool CanWrite(void) const
        {
            return m_Stream.get() != 0;
        }

    CNcbiOstream& operator*(void)
        {
            _ASSERT(m_Stream.get());
            return *m_Stream;
        }

    void Close(void)
        {
            *m_Stream << flush;
            if ( !*m_Stream ) {
                Abort();
            }
            m_Stream.reset();
            m_Writer.reset();
        }

    void Abort(void)
        {
            m_Stream.reset();
            m_Writer.reset();
            Remove();
        }

    void Remove(void)
        {
            try {
                m_Cache->Remove(m_Key, m_Version, m_Subkey);
            }
            catch ( exception& ) { // ignored
            }
        }

private:
    ICache*             m_Cache;
    string              m_Key;
    TVersion            m_Version;
    string              m_Subkey;
    auto_ptr<IWriter>   m_Writer;
    auto_ptr<CWStream>  m_Stream;
};


CRef<CWriter::CBlobStream>
CCacheWriter::OpenBlobStream(CReaderRequestResult& result,
                             const TBlobId& blob_id,
                             TChunkId chunk_id,
                             const CProcessor& processor)
{
    if( !m_BlobCache ) {
        return null;
    }

    try {
        CLoadLockBlob blob(result, blob_id, chunk_id);
        TBlobVersion version = blob.GetKnownBlobVersion();
        if ( version < 0 ) {
            CLoadLockBlobVersion version_lock(result, blob_id, eAlreadyLoaded);
            if ( version_lock ) {
                version = version_lock.GetBlobVersion();
            }
        }
        _ASSERT(version >= 0);
        CRef<CBlobStream> stream
            (new CCacheBlobStream(m_BlobCache, GetBlobKey(blob_id),
                                  version,
                                  GetBlobSubkey(blob, chunk_id)));
        if ( !stream->CanWrite() ) {
            return null;
        }
        
        WriteProcessorTag(**stream, processor);
        return stream;
    }
    catch ( exception& ) { // ignored
        return null;
    }
}


bool CCacheWriter::CanWrite(EType type) const
{
    return (type == eIdWriter ? m_IdCache : m_BlobCache) != 0;
}


END_SCOPE(objects)


using namespace objects;


/// Class factory for Cache writer
///
/// @internal
///
class CCacheWriterCF :
    public CSimpleClassFactoryImpl<CWriter, CCacheWriter>
{
private:
    typedef CSimpleClassFactoryImpl<CWriter, CCacheWriter> TParent;
public:
    CCacheWriterCF()
        : TParent(NCBI_GBLOADER_WRITER_CACHE_DRIVER_NAME, 0) {}
    ~CCacheWriterCF() {}


    CWriter*
    CreateInstance(const string& driver  = kEmptyStr,
                   CVersionInfo version = NCBI_INTERFACE_VERSION(CWriter),
                   const TPluginManagerParamTree* params = 0) const
    {
        if ( !driver.empty()  &&  driver != m_DriverName )
            return 0;

        if ( !version.Match(NCBI_INTERFACE_VERSION(CWriter)) ) {
            return 0;
        }
        return new CCacheWriter();
    }
};


void NCBI_EntryPoint_CacheWriter(
     CPluginManager<CWriter>::TDriverInfoList&   info_list,
     CPluginManager<CWriter>::EEntryPointRequest method)
{
    CHostEntryPointImpl<CCacheWriterCF>::NCBI_EntryPointImpl(info_list,
                                                             method);
}


void NCBI_EntryPoint_xwriter_cache(
     CPluginManager<CWriter>::TDriverInfoList&   info_list,
     CPluginManager<CWriter>::EEntryPointRequest method)
{
    NCBI_EntryPoint_CacheWriter(info_list, method);
}


void GenBankWriters_Register_Cache(void)
{
    RegisterEntryPoint<CWriter>(NCBI_EntryPoint_CacheWriter);
}


END_NCBI_SCOPE
