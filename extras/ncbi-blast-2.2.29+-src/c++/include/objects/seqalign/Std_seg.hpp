/* $Id: Std_seg.hpp 207227 2010-10-04 15:26:24Z grichenk $
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
 * Author:  .......
 *
 * File Description:
 *   .......
 *
 * Remark:
 *   This code was originally generated by application DATATOOL
 *   using specifications from the data definition file
 *   'seqalign.asn'.
 */

#ifndef OBJECTS_SEQALIGN_STD_SEG_HPP
#define OBJECTS_SEQALIGN_STD_SEG_HPP


// generated includes
#include <objects/seqalign/Std_seg_.hpp>
#include <util/range.hpp>

// generated classes

BEGIN_NCBI_SCOPE

BEGIN_objects_SCOPE // namespace ncbi::objects::

class CSeq_interval;

class NCBI_SEQALIGN_EXPORT CStd_seg : public CStd_seg_Base
{
    typedef CStd_seg_Base Tparent;
public:
    // constructor
    CStd_seg(void);
    // destructor
    ~CStd_seg(void);

    // Validators
    TDim CheckNumRows(void)                   const;
    void Validate    (bool full_test = false) const;

    // GetSeqRange
    TSignedSeqPos         GetSeqStart(TDim row) const;
    TSignedSeqPos         GetSeqStop (TDim row) const;
    CRange<TSignedSeqPos> GetSeqRange(TDim row) const;
    
    /// Swap two rows (changing *order*, not content)
    void SwapRows(TDim row1, TDim row2);


    /// Offset row's coords
    void OffsetRow(TDim row, TSignedSeqPos offset);

    /// @deprecated
    NCBI_DEPRECATED void RemapToLoc(TDim row,
                                    const CSeq_loc& dst_loc,
                                    bool ignore_strand = false);

    CRef<CSeq_loc> CreateRowSeq_loc(TDim row) const;

private:
    // Prohibit copy constructor and assignment operator
    CStd_seg(const CStd_seg& value);
    CStd_seg& operator=(const CStd_seg& value);

};



/////////////////// CStd_seg inline methods

// constructor
inline
CStd_seg::CStd_seg(void)
{
}


/////////////////// end of CStd_seg inline methods


END_objects_SCOPE // namespace ncbi::objects::

END_NCBI_SCOPE

#endif // OBJECTS_SEQALIGN_STD_SEG_HPP
/* Original file checksum: lines: 93, chars: 2392, CRC32: 7f149965 */
