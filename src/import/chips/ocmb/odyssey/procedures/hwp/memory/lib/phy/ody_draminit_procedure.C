/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/ocmb/odyssey/procedures/hwp/memory/lib/phy/ody_draminit_procedure.C $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2024                        */
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
// EKB-Mirror-To: hostboot
///
/// @file ody_draminit_procedure.C
/// @brief Odyssey draminit procedure
/// @note Using a separate file as simulation might need a different draminit procedure for now
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2
// *HWP Consumed by: FSP:HB

#include <fapi2.H>

#include <generic/memory/lib/utils/find.H>
#include <generic/memory/lib/utils/c_str.H>

#include <lib/phy/ody_draminit_utils.H>
#include <lib/phy/ody_phy_utils.H>
#include <lib/phy/ody_draminit_procedure.H>
#include <generic/memory/lib/utils/fir/gen_mss_unmask.H>

namespace mss
{
namespace ody
{

///
/// @brief Runs draminit
/// @param[in] i_target the target on which to operate
/// @param[out] o_log_data hwp_data_ostream of streaming log
/// @return fapi2::FAPI2_RC_SUCCESS iff successful
/// @note Assumes PHY init has already been run
///
fapi2::ReturnCode draminit(const fapi2::Target<fapi2::TARGET_TYPE_MEM_PORT>& i_target,
                           fapi2::hwp_data_ostream& o_log_data)
{

    // Create the stucture to configure
    PMU_SMB_DDR5U_1D_t l_msg_block;
    uint64_t l_status = mss::ody::phy::mailbox_consts::FAILED_COMPLETION;
    uint8_t l_draminit_step_enable = 0;
    fapi2::ATTR_DRAMINIT_TRAINING_TIMEOUT_Type l_poll_count;

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_DRAMINIT_TRAINING_TIMEOUT, i_target , l_poll_count));
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_ODY_DRAMINIT_STEP_ENABLE, fapi2::Target<fapi2::TARGET_TYPE_SYSTEM>(),
                           l_draminit_step_enable));

    // 1. Loads the IMEM Memory (instructions) onto Synopsys PHY
    // It is being done in ody_load_imem.C

    // 2. Loads the DMEM Memory (data) onto Synopsys PHY
    // It is being done in ody_load_dmem.C

    // 2.1 Sets the attribute to note that the DMEM has been loaded
    // Due to memory size constraints on the SBE, the dmem load procedures can be called multiple times
    // As the speed ups for this attribute depend upon the YES enum, the NO is set here after all loads have completed
    FAPI_TRY(FAPI_ATTR_SET_CONST(fapi2::ATTR_ODY_DMEM_FIRST_LOAD, i_target, fapi2::ENUM_ATTR_ODY_DMEM_FIRST_LOAD_NO));

    // 3. Configures and loads the message block onto Synopsys PHY
    if (mss::ody::skip_this_step(fapi2::ENUM_ATTR_ODY_DRAMINIT_STEP_ENABLE_LOAD_MSG_BLOCK, l_draminit_step_enable))
    {
        FAPI_INF(TARGTIDFORMAT " ATTR_ODY_DRAMINIT_STEP_ENABLE set to skip message block loading...", TARGTID);
        FAPI_TRY(mss::ody::phy::configure_dram_train_message_block(i_target, l_msg_block));
    }
    else
    {
        FAPI_TRY(mss::ody::phy::configure_and_load_dram_train_message_block(i_target, l_msg_block));
        mss::ody::phy::display_msg_block(i_target, l_msg_block);
    }

    if (mss::ody::skip_this_step(fapi2::ENUM_ATTR_ODY_DRAMINIT_STEP_ENABLE_RUN_TRAINING, l_draminit_step_enable))
    {
        FAPI_INF(TARGTIDFORMAT " ATTR_ODY_DRAMINIT_STEP_ENABLE set to skip running DRAM training...", TARGTID);
    }
    else
    {
        // Array for keeping track of the starting bad bits
        // The bytes are in the MC perspective, while the ranks and bits are in the PHY perspective
        uint8_t l_start_bad_bits[BAD_BITS_RANKS][BAD_DQ_BYTE_COUNT] =
        {
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        };

        // run_training_helper executes steps 4, 5, 6, and 7a
        FAPI_TRY(mss::ody::phy::run_training_helper(i_target, l_status, l_start_bad_bits, l_msg_block, o_log_data));
        mss::ody::phy::display_msg_block(i_target, l_msg_block);

        // Handle any error recovery cases here
        FAPI_TRY(mss::ody::phy::handle_draminit_recovery(i_target,
                 l_status,
                 l_start_bad_bits,
                 l_msg_block,
                 o_log_data));

        // 7b. Load attibutes with the message block contents
        FAPI_TRY(mss::ody::phy::set_attributes(i_target, l_msg_block));

        // 8. Error handling
        FAPI_TRY(mss::ody::phy::check_training_result(i_target, l_status, l_msg_block));
    }

    // 9. Load PHY Initialization Engine image and set up mission mode settings
    // This is done in ody_load_pie.C

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Runs draminit
/// @param[in] i_target the target on which to operate
/// @param[out] o_log_data hwp_data_ostream of streaming log
/// @return fapi2::FAPI2_RC_SUCCESS iff successful
/// @note Assumes PHY init has already been run
///
fapi2::ReturnCode draminit(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                           fapi2::hwp_data_ostream& o_log_data)
{
    fapi2::ReturnCode l_rc = fapi2::FAPI2_RC_SUCCESS;

    for(const auto& l_port : mss::find_targets<fapi2::TARGET_TYPE_MEM_PORT>(i_target))
    {
        fapi2::ReturnCode l_port_rc = fapi2::FAPI2_RC_SUCCESS;

        // Allow training to fail on one port but continue on the second
        l_port_rc = draminit(l_port, o_log_data);

        if (l_port_rc != fapi2::FAPI2_RC_SUCCESS)
        {
            // Log error from previous port if it failed
            if (l_rc != fapi2::FAPI2_RC_SUCCESS)
            {
                fapi2::logError(l_rc, fapi2::FAPI2_ERRL_SEV_UNRECOVERABLE);
            }

            // Set HWP RC to last port's fail so it gets logged upon HWP exit
            l_rc = l_port_rc;
        }
    }

    FAPI_TRY(l_rc, TARGTIDFORMAT " failure during draminit", TARGTID);

    // Unmask FIRs (done on the OCMB chip level)
    FAPI_TRY(mss::unmask::after_draminit_training<mss::mc_type::ODYSSEY>(i_target));

fapi_try_exit:
    return fapi2::current_err;
}

} // namespace ody
} // namespace mss
