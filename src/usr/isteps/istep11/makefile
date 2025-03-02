# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: src/usr/isteps/istep11/makefile $
#
# OpenPOWER HostBoot Project
#
# Contributors Listed Below - COPYRIGHT 2015,2024
# [+] International Business Machines Corp.
#
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied. See the License for the specific language governing
# permissions and limitations under the License.
#
# IBM_PROLOG_END_TAG
ROOTPATH = ../../../..
MODULE = istep11

# P10 procedures path
PROCEDURES_PATH = ${ROOTPATH}/src/import/chips/p10/procedures

# OCMB paths
COMMON_PATH_OCMB = ${ROOTPATH}/src/import/chips/ocmb/common
EXPLORER_PATH_OCMB = ${ROOTPATH}/src/import/chips/ocmb/explorer

# Extra include paths
EXTRAINCDIR += ${COMMON_PATH_OCMB}/procedures/hwp/pmic/
EXTRAINCDIR += ${COMMON_PATH_OCMB}/procedures/hwp/pmic/lib/
EXTRAINCDIR += ${COMMON_PATH_OCMB}/procedures/hwp/pmic/lib/utils/
EXTRAINCDIR += ${COMMON_PATH_OCMB}/procedures/hwp/pmic_ddr5/
EXTRAINCDIR += ${COMMON_PATH_OCMB}/procedures/hwp/pmic_ddr5/lib/utils/
EXTRAINCDIR += ${COMMON_PATH_OCMB}/procedures/hwp/pmic/lib/i2c/
EXTRAINCDIR += ${COMMON_PATH_OCMB}/include/
EXTRAINCDIR += ${PROCEDURES_PATH}/hwp/perv/
EXTRAINCDIR += ${PROCEDURES_PATH}/hwp/ffdc/
EXTRAINCDIR += ${EXPLORER_PATH_OCMB}/procedures/hwp/memory/
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/common/utils/
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/common/utils/scomt/
EXTRAINCDIR += ${ROOTPATH}/src/import/hwpf/fapi2/include/
EXTRAINCDIR += ${ROOTPATH}/src/include/usr/fapi2/
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/p10/common/include/
EXTRAINCDIR += ${ROOTPATH}/src/import/
EXTRAINCDIR += ${ROOTPATH}/src/import/generic/memory/lib/
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/p10/utils/imageProcs/
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/common/utils/imageProcs/
EXTRAINCDIR += ${ROOTPATH}/src/usr/isteps/
EXTRAINCDIR += ${ROOTPATH}/obj/genfiles/chips/ocmb/explorer/procedures/hwp/memory/lib/
EXTRAINCDIR += ${ROOTPATH}/obj/genfiles/chips/ocmb/common/procedures/hwp/pmic/lib/
EXTRAINCDIR += ${ROOTPATH}/obj/genfiles/generic/memory/lib/
EXTRAINCDIR += ${ROOTPATH}/obj/genfiles/chips/ocmb/odyssey/procedures/hwp/memory/lib/
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/ocmb/odyssey/procedures/hwp/memory/
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/ocmb/odyssey/common/include/
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/ocmb/odyssey/procedures/hwp/perv/
EXTRAINCDIR += ${ROOTPATH}/src/include/usr/ocmbupd/
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/p10/procedures/hwp/sbe/

# Istep objs
OBJS += call_host_prd_hwreconfig.o
OBJS += call_host_set_mem_volt.o
OBJS += call_proc_ocmb_enable.o
OBJS += call_ocmb_check_for_ready.o

# HWP objs
OBJS += p10_ocmb_enable.o
OBJS += exp_check_for_ready.o
OBJS += pmic_enable.o
OBJS += pmic_enable_utils.o
OBJS += pmic_common_utils.o
OBJS += mss_pmic_attribute_accessors_manual.o
OBJS += pmic_enable_utils_ddr5.o
OBJS += pmic_common_utils_ddr4.o
OBJS += pmic_common_utils_ddr5.o
OBJS += pmic_health_check_utils_ddr5.o
OBJS += pmic_periodic_telemetry_utils_ddr5.o
OBJS += pmic_health_check_ddr5.o
OBJS += pmic_periodic_telemetry_ddr5.o
OBJS += ody_check_for_ready.o
OBJS += ody_sppe_config_update.o
OBJS += ody_scratch_regs.o
OBJS += ody_scratch_regs_utils.o
OBJS += ody_cbs_start.o
OBJS += poz_perv_mod_misc.o
OBJS += poz_perv_utils.o

VPATH += ${PROCEDURES_PATH}/hwp/perv/
VPATH += ${EXPLORER_PATH_OCMB}/procedures/hwp/memory/
VPATH += ${COMMON_PATH_OCMB}/procedures/hwp/pmic/
VPATH += ${COMMON_PATH_OCMB}/procedures/hwp/pmic/lib/utils/
VPATH += ${COMMON_PATH_OCMB}/procedures/hwp/pmic/lib/
VPATH += ${COMMON_PATH_OCMB}/procedures/hwp/pmic_ddr5/
VPATH += ${COMMON_PATH_OCMB}/procedures/hwp/pmic_ddr5/lib/utils/
VPATH += ${ROOTPATH}/src/import/chips/ocmb/odyssey/procedures/hwp/memory/
VPATH += ${ROOTPATH}/src/import/chips/ocmb/odyssey/procedures/hwp/perv/

include ${ROOTPATH}/config.mk
