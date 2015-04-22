/*  $Id: named_parameters.hpp 398199 2013-05-03 16:52:58Z kazimird $
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
 * Author:  Dmitry Kazimirov
 *
 * File Description: Support for named parameters in methods.
 *
 */


#ifndef CONNECT_SERVICES__NAMED_PARAMETERS_HPP
#define CONNECT_SERVICES__NAMED_PARAMETERS_HPP

#include <corelib/ncbistd.hpp>

BEGIN_NCBI_SCOPE

class CNamedParameterList
{
public:
    CNamedParameterList(int tag) : m_Tag(tag), m_MoreParams(NULL) {}

    operator const CNamedParameterList*() const {return this;}

    const CNamedParameterList& operator ,(
            const CNamedParameterList& more_params) const
    {
        more_params.m_MoreParams = this;
        return more_params;
    }

    bool Is(int tag) const {return m_Tag == tag;}

    int m_Tag;
    mutable const CNamedParameterList* m_MoreParams;
};

template <typename TYPE>
class CNamedParameterValue : public CNamedParameterList
{
public:
    CNamedParameterValue(int tag, const TYPE& value) :
        CNamedParameterList(tag),
        m_Value(value)
    {
    }

    TYPE m_Value;
};

template <typename TYPE, int TAG>
class CNamedParameter
{
public:
    CNamedParameterValue<TYPE> operator =(const TYPE& value)
    {
        return CNamedParameterValue<TYPE>(TAG, value);
    }
};

template <typename TYPE>
const TYPE& Get(const CNamedParameterList* param)
{
    return static_cast<const CNamedParameterValue<TYPE>*>(param)->m_Value;
}

END_NCBI_SCOPE

#endif  /* CONNECT_SERVICES__NAMED_PARAMETERS_HPP */
