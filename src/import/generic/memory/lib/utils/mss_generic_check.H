/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/generic/memory/lib/utils/mss_generic_check.H $     */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2018,2024                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */

///
/// @file mss_generic_check.H
/// @brief Checker functions for generic API
///
// *HWP HWP Owner: Louis Stermole <stermole@us.ibm.com>
// *HWP HWP Backup: Stephen Glancy <sglancy@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 3
// *HWP Consumed by: HB:FSP

#ifndef _MSS_GENERIC_CHECK_H_
#define _MSS_GENERIC_CHECK_H_

#include <fapi2.H>


#ifdef __PPE__
    #include <c_str.H>
#else
    #include <generic/memory/lib/utils/c_str.H>
#endif

namespace mss
{
namespace check
{

///
/// @brief Making an enum for switch/case in bad_fir_bits. Both will need
///       to be updated if additional FIR checklists are needed
///
enum firChecklist {GENERIC, OMI, DRAMINIT, CCS, IO_GENERAL, IO_TRAIN};


///
/// @brief Helper function to log an error as related if needed
/// @tparam T the target type on which to log the error
/// @param[in] i_target - the target to log the error on
/// @param[in] i_fir_error - true IFF a fir is present on the passed in target
/// @param[in,out] io_rc - the return code to log
///
template< fapi2::TargetType T >
inline void log_fir_helper( const fapi2::Target<T>& i_target,
                            const bool i_fir_error,
                            fapi2::ReturnCode& io_rc)
{
    if(i_fir_error && (io_rc != fapi2::FAPI2_RC_SUCCESS))
    {
        // PPE does not implement log_related_error
#ifndef __PPE__
        fapi2::log_related_error(i_target, io_rc, fapi2::FAPI2_ERRL_SEV_RECOVERED);

        io_rc = fapi2::FAPI2_RC_SUCCESS;
#else
        FAPI_ERR(TARGTIDFORMAT " HWP fail with related FIR bits set!", TARGTID);
#endif
    }
}

///
/// @brief Checks whether any FIRs have lit up on a target
/// @tparam MC MC type for which to check FIR's
/// @tparam F the FIR reg checklist to be checked
/// @tparam T the fapi2::TargetType which hold the FIR bits
/// @param[in] i_target - the target on which to operate
/// @param[in,out] io_rc - the return code for the function
/// @param[out] o_fir_error - true iff a FIR was hit
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff ok
///
template< mss::mc_type MC, firChecklist F, fapi2::TargetType T >
fapi2::ReturnCode bad_fir_bits( const fapi2::Target<T>& i_target, fapi2::ReturnCode& io_rc, bool& o_fir_error);

///
/// @brief Checks whether the passed in FIRs have any un-masked errors set
/// @param[in] i_fir - the contents of the fir register to be checked
/// @param[in] i_mask - the contents of the fir mask to be checked
/// @return bool return true iff an unmasked fir bit is found
///
inline bool fir_with_mask_helper( const fapi2::buffer<uint64_t>& i_fir,
                                  const fapi2::buffer<uint64_t>& i_mask)
{
    fapi2::buffer<uint64_t> l_fir_mask(i_mask);

    // The mask register will need to be inverted as a 0 in the mask register means the FIR is legit
    // A bitwise and works the opposite way
    l_fir_mask.invert();

    // Return true if any fir is set and unmasked to indicate this is a "new" FIR
    return ((i_fir & l_fir_mask) != 0);
}

///
/// @brief Checks whether the passed in FIRs have any un-masked errors set
/// @tparam MC MC type for which to check FIR's
/// @tparam T the fapi2::TargetType which hold the FIR bits
/// @param[in] i_target - the target on which to operate
/// @param[in] i_fir_addr - address of the FIR register
/// @param[in] i_mask_addr - address of the mask register for i_fir_addr
/// @param[out] o_fir_error - true iff a FIR was hit
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff ok
///
template< mss::mc_type MC, fapi2::TargetType T >
inline fapi2::ReturnCode fir_with_mask( const fapi2::Target<T>& i_target,
                                        const uint64_t i_fir_addr,
                                        const uint64_t i_mask_addr,
                                        bool& o_fir_error )
{
    fapi2::buffer<uint64_t> l_fir;
    fapi2::buffer<uint64_t> l_fir_mask;

    // Read the registers
    FAPI_TRY(fapi2::getScom(i_target, i_fir_addr, l_fir));
    FAPI_TRY(fapi2::getScom(i_target, i_mask_addr, l_fir_mask));

    // If we have any unmasked bit, set that we have a FIR error and exit out with TRUE
    // Note: we want to set TRUE here as PRD will find the FIR as "new" and retrigger the procedure this way
    o_fir_error = fir_with_mask_helper(l_fir, l_fir_mask);

    // And print the information for debuggability
    FAPI_INF_NO_SBE("%s %s on reg 0x%016lx value 0x%016lx and mask 0x%016lx value 0x%016lx", mss::c_str(i_target),
                    o_fir_error ? "has FIR's set" : "has no FIR's set", i_fir_addr, l_fir, i_mask_addr, l_fir_mask);

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Checks whether a FIR or unlocked PLL could be the root cause of another failure
/// @tparam MC MC type for which to check FIR's
/// @tparam F the FIR reg checklist to be checked
/// @tparam T the fapi2::TargetType which hold the FIR bits
/// @param[in] i_target - the target on which to operate
/// @param[in,out] io_rc - the return code for the function
/// @param[in,out] io_fircheck_scom_err - the return code for reading the FIR scoms
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff ok
/// @note This is a helper function to enable unit testing
///
template< mss::mc_type MC, firChecklist F, fapi2::TargetType T >
fapi2::ReturnCode hostboot_fir_or_pll_fail( const fapi2::Target<T>& i_target,
        fapi2::ReturnCode& io_rc,
        fapi2::ReturnCode& io_fircheck_scom_err)
{
    // We didn't have an error, so return success
    if(io_rc == fapi2::FAPI2_RC_SUCCESS)
    {
        FAPI_INF_NO_SBE(TARGTIDFORMAT " has a good return code, returning success", TARGTID);
        return fapi2::FAPI2_RC_SUCCESS;
    }

    bool l_fir_error = false;

#ifndef __PPE__
    FAPI_ERR(TARGTIDFORMAT " has a bad return code, time to check some firs!", TARGTID);
#endif

    // Check the fir bits against the mask and return fir status thru fapi2 RC
    io_fircheck_scom_err = bad_fir_bits<MC, F>(i_target, io_rc, l_fir_error);

#ifndef __PPE__
    FAPI_ERR(TARGTIDFORMAT " took a fail. FIR was %s", TARGTID,
             l_fir_error ? "set - returning FIR RC" : "unset - returning inputted RC");
#endif

    // If we had a FIR error, log the original error and return success
    // PRD will handle the original error
    if(l_fir_error)
    {
        fapi2::current_err = fapi2::FAPI2_RC_SUCCESS;
    }
    else
    {
        fapi2::current_err = io_rc;
    }

    return fapi2::current_err;
}

///
/// @brief Checks whether a FIR or unlocked PLL could be the root cause of another failure, if a check fir boolean is passed in
/// @tparam MC MC type for which to check FIR's
/// @tparam F the FIR reg checklist to be checked
/// @tparam T the fapi2::TargetType which hold the FIR bits
/// @param[in] i_target - the target on which to operate
/// @param[in,out] io_rc - the return code for the function
/// @param[in] i_check_fir - true IFF the FIR needs to be checked - defaults to true
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff ok
///
template< mss::mc_type MC, firChecklist F, fapi2::TargetType T >
fapi2::ReturnCode fir_or_pll_fail( const fapi2::Target<T>& i_target,
                                   fapi2::ReturnCode& io_rc,
                                   const bool i_check_fir = true )
{
#if defined(__HOSTBOOT_MODULE) || defined(__PPE__)

    fapi2::ReturnCode l_rc(io_rc);
    fapi2::ReturnCode l_fircheck_scom_err(fapi2::FAPI2_RC_SUCCESS);

    // If need be, check the FIR below
    if(i_check_fir)
    {
        // Handle any issues according to PRD FIR scheme, as a FIR could have caused this issue
        l_rc = hostboot_fir_or_pll_fail<MC, F>(i_target, l_rc, l_fircheck_scom_err);
    }

    // If we encountered a scom error while checking FIRs, return that RC
    if (l_fircheck_scom_err != fapi2::FAPI2_RC_SUCCESS)
    {
        return l_fircheck_scom_err;
    }

    return l_rc;

#else
    return io_rc;
#endif
}

///
/// @brief Check GENERIC FIR checklist to see if we can blame any FIRs for a given DRAM training error code
/// @tparam MC MC type on which training was run
/// @tparam T fapi2::TargetType on which training was conducted
/// @param[in] i_target the fapi2 target on which training was conducted
/// @param[out] o_firs_found set to true if a FIR has been found set in the checklist
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff successful
/// @note calling GENERIC bad_fir_bits to cover this case in HB -- GENERIC firChecklist not specified (see bad_fir_bits for more details)
///
template <mss::mc_type MC, fapi2::TargetType T >
inline fapi2::ReturnCode blame_firs( const fapi2::Target<T>& i_target, bool& o_firs_found )
{
    o_firs_found = false;

    // If we have a FIR set that could have caused our training fail, then skip checking bad bits in FW
    // PRD will handle the FIR and retrigger the procedure
#if defined(__HOSTBOOT_MODULE) || defined(__PPE__)

    // Note: using success here will cause an RC to not be logged
    // We can still see if we do have a FIR error though
    // NOTE: calling GENERIC bad_fir_bits to cover this case in HB -- GENERIC firChecklist not specified (see bad_fir_bits for more details)
    fapi2::ReturnCode l_rc(fapi2::FAPI2_RC_SUCCESS);
    FAPI_TRY((mss::check::bad_fir_bits<MC, mss::check::firChecklist::GENERIC, T>(i_target, l_rc, o_firs_found)),
             TARGTIDFORMAT " took an error while checking FIR's", TARGTID);

    // Exit if we took a FIR error - PRD will handle bad bits
    if(o_firs_found)
    {
        FAPI_INF(TARGTIDFORMAT " has FIR's set, exiting to let PRD handle it", TARGTID);
        return fapi2::FAPI2_RC_SUCCESS;
    }

#endif

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Checks conditional passes and implements traces & exits if it fails
/// @tparam T fapi2 target type
/// @tparam IT input data type
/// @tparam FFDC error callout code type
/// @param[in] i_target fapi2 target
/// @param[in] i_conditional conditional that we are testing against
/// @param[in] i_byte_index byte index
/// @param[in] i_data debug data
/// @param[in] i_ffdc_codes FFDC code
/// @param[in] i_err_str error string - defaulted to ""
/// @return FAPI2_RC_SUCCESS iff okay
///
template< fapi2::TargetType T, typename IT , typename FFDC >
inline fapi2::ReturnCode invalid_value(const fapi2::Target<T>& i_target,
                                       const bool i_conditional,
                                       const size_t i_byte_index,
                                       const IT i_data,
                                       const FFDC i_ffdc_codes,
                                       const char* i_err_str = "")
{
    FAPI_ASSERT(i_conditional,
                fapi2::MSS_FAILED_DATA_INTEGRITY_CHECK().
                set_VALUE(i_data).
                set_BYTE(i_byte_index).
                set_TARGET(i_target).
                set_FFDC_CODE(i_ffdc_codes),
                "%s. Byte %d, Data returned: %d for " TARGTIDFORMAT,
                i_err_str, i_byte_index, i_data, TARGTID );

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;

} // invalid_value

#ifndef __PPE__
///
/// @brief Checks conditional passes and implements traces & exits if it fails
/// @tparam OT data type we want to check safe conversion against (explicit)
/// @tparam T fapi2 target type
/// @tparam IT input data type
/// @tparam FFDC error callout code type
/// @param[in] i_target fapi2 target
/// @param[in] i_conditional conditional that we are testing against
/// @param[in] i_byte_index byte index
/// @param[in] i_data debug data
/// @param[in] i_ffdc_codes FFDC code
/// @param[in] i_err_str error string - defaulted to ""
/// @return FAPI2_RC_SUCCESS iff okay
///
template< typename OT, fapi2::TargetType T, typename IT, typename FFDC >
inline fapi2::ReturnCode invalid_type_conversion(const fapi2::Target<T>& i_target,
        const IT& i_data,
        const FFDC i_ffdc_codes,
        const char* i_err_str = "")
{
    OT l_temp = static_cast<OT>(i_data);

    FAPI_ASSERT( i_data == l_temp,
                 fapi2::MSS_CONVERSION_ERROR()
                 .set_ORIGINAL_VAL(i_data)
                 .set_CONVERTED_VAL(l_temp)
                 .set_TARGET(i_target)
                 .set_FUNCTION(i_ffdc_codes),
                 "%s. Conversion error between original %d to converted %d value for " TARGTIDFORMAT,
                 i_err_str, i_data, l_temp, TARGTID );

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;

} // invalid_type_conversion
#endif

///
/// @brief Checks conditional passes and implements traces & exits if it fails
/// @tparam T fapi2 target type
/// @tparam IT index type
/// @tparam ST size type
/// @tparam FFDC error callout code type
/// @param[in] i_target fapi2 target
/// @param[in] i_index desired vector index
/// @param[in] i_size size of the list we are comparing
/// @param[in] i_ffdc_codes FFDC code
/// @param[in] i_err_str error string - defaulted to ""
/// @return FAPI2_RC_SUCCESS iff okay
///
template< fapi2::TargetType T, typename IT, typename ST, typename FFDC >
fapi2::ReturnCode index_within_bounds(const fapi2::Target<T>& i_target,
                                      const IT i_index,
                                      const ST i_size,
                                      const FFDC i_ffdc_codes,
                                      const char* i_err_str = "")
{
    FAPI_ASSERT( i_index < i_size,
                 fapi2::MSS_OUT_OF_BOUNDS_INDEXING()
                 .set_INDEX(i_index)
                 .set_LIST_SIZE(i_size)
                 .set_FUNCTION(i_ffdc_codes)
                 .set_TARGET(i_target),
                 "%s. Out of bounds indexing (with %d) on a list of size %d for " TARGTIDFORMAT,
                 i_err_str, i_index, i_size, TARGTID );

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;

} // invalid_type_conversion

} // check
}// mss

#endif
