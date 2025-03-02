/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/include/usr/errl/hberrltypes.H $                          */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2012,2023                        */
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
#ifndef HBERRLTYPES_H
#define HBERRLTYPES_H
/**
 *  @file errltypes.H
 *
 *  @brief  Common ErrlEntry definitions.
 *
 *  This header file contains the definition of ErrlEntry related
 *  parameters.
 *
 *  This file will be included by error log parsers that run
 *  on pool machines, so the stuff in here needs to be generic.
 *
 */
#include <hbotcompid.H>             // compId_t
#include <stdint.h>
#include <string.h>                 // strlen
#include <vector>

/**
 * @brief Pointer to an ErrlEntry object
 */
namespace ERRORLOG { class ErrlEntry; };

/**
 * @brief Macros to store different numbers into uint64_t
 */

// Convert any integer to uint16_t
#define TO_UINT16(x)    static_cast<uint16_t>(x)

// Convert any integer to uint32_t
#define TO_UINT32(x)    static_cast<uint32_t>(x)

// Convert any integer to uint64_t
#define TO_UINT64(x)    static_cast<uint64_t>(x)

// uint16_t val = 8bit:8bit
#define TWO_UINT8_TO_UINT16(left_8, right_8) \
            ( (TO_UINT16(left_8) << 8) | TO_UINT16(right_8) )

// uint32_t val = 16bit:16bit
#define TWO_UINT16_TO_UINT32(left_16, right_16) \
            ( (TO_UINT32(left_16) << 16) | TO_UINT32(right_16) )

// uint64_t val = 32bit:32bit
#define TWO_UINT32_TO_UINT64(left_32, right_32) \
            ( (TO_UINT64(left_32) << 32) | TO_UINT64(right_32) )

// uint64_t val = 16bit:16bit:32bit
#define TWO_UINT16_ONE_UINT32_TO_UINT64(left_16, mid_16, right_32) \
            ( (TO_UINT64(TWO_UINT16_TO_UINT32(left_16, mid_16)) << 32) | \
              TO_UINT64(right_32) )

// uint64_t val = 16bit:16bit:16bit:16bit
#define FOUR_UINT16_TO_UINT64(mostleft_16, left_16, right_16, mostright_16) \
        ( (TO_UINT64(TWO_UINT16_TO_UINT32(mostleft_16, left_16)) << 32) | \
          (TO_UINT64(TWO_UINT16_TO_UINT32(right_16, mostright_16))) )

// uint32_t val = 8bit:8bit:8bit:8bit
#define FOUR_UINT8_TO_UINT32(mostleft_8, left_8, right_8, mostright_8) \
        ( (TO_UINT32(TWO_UINT8_TO_UINT16(mostleft_8, left_8)) << 16) | \
          (TO_UINT32(TWO_UINT8_TO_UINT16(right_8, mostright_8))) )

namespace errl_util
{

/** @brief Generate a right-justified bitmask with the given number of
 *  bits set to 1.
 */
inline uint64_t right_bitmask(const int bits)
{
    if (bits == 64)
    {
        return 0ull - 1;
    }

    return ((1ull << bits) - 1);
}

/** @brief Base case for recursion.
 */
inline uint64_t SrcUserData_(const uint64_t accumulator)
{
    return accumulator;
}

struct bits
{
    int start = 0, end = 0;
};

/** @brief Set the given bits in [accumulator] to [value].
 *  Then do the same thing for the rest of the arguments.
 */
template<typename T, typename... Args>
uint64_t SrcUserData_(const uint64_t accumulator,
                   const bits range, const T value,
                   Args&&... rest)
{
    return SrcUserData_(accumulator
                        | ((static_cast<uint64_t>(value)
                            & right_bitmask(range.end - range.start + 1))
                           << (64 - range.end - 1)),
                        std::forward<Args>(rest)...);
}

/**
 * @brief Create a 64-bit userdata value for an error log from a list
 * of bit ranges and values.
 *
 * Example:
 *
 *   SrcUserData(bits{0 ,  7}, value1,
 *               bits{8 , 15}, value2,
 *               bits{16, 31}, value3,
 *               bits{32, 33}, value4,
 *               bits{34, 63}, value5)
 *
 * In this example, the bit range [0, 7] will be set to the lower 8
 * bits of value1, the bit range [32, 33] will be set to the lower 2
 * bits of value4, etc. Any starting and ending positions can be used
 * (as long as they fit in 64 bits), and any number of ranges/values
 * can be used, and any types of values can be used, as long as each
 * type is convertible to uint64_t. The starting and ending bit
 * positions are inclusive.
 */
template<typename... Args>
uint64_t SrcUserData(Args&&... args)
{
    return SrcUserData_(0, std::forward<Args>(args)...);
}

}

namespace ERRORLOG
{


/**
 *    @brief Scope is found in user header section.
 */
enum errlScope_t
{
    ERRL_SCOPE_PLATFORM            = 0x03, ///< Entire Platform
};


/**
 *   @brief Domain as found in user header section.
 */
enum errlDomain_t
{
    ERRL_DOMAIN_DEFAULT     = 0xFF  // Field based on Subsystem ID
};


/**
 *   @brief Vector as found in user header section.
 */
enum errlVector_t
{
    ERRL_VECTOR_DEFAULT      = 0xFF  // Vector that triggered the base class
                                     // of problem in Problem Domain Field.
                                     // Unique field based on Subsystem ID
};



/**
 *  @brief Platform Event Log actions
 *
 *  Several action flags used to trigger various
 *  handling mechanisms for an event log such as
 *  calling home and sending to the hypervisor.
 *  Naming these ERRL_ACTIONS because they are
 *  used in the prdf code with different values
 */
enum errlActions_t
{
    ERRL_ACTIONS_NONE             = 0x0000, // No action required
    ERRL_ACTIONS_REPORT           = 0x2000, // Report Externally (Send to HMC and Hypervisor)
    ERRL_ACTIONS_SA               = 0x8000, // Service Action (Customer Notification) Required, Report bit automatically set
    ERRL_ACTIONS_HIDDEN           = 0x4000, // Hidden Error - exclusive with SA Required, Report bit automatically set
    ERRL_ACTIONS_REPORT_HMC_ONLY  = 0x1000, // Report to HMC only (Only report to HMC), Report bit automatically set
    ERRL_ACTIONS_REPORT_HYP_ONLY  = 0x0200, // Report to Hypervisor only
    ERRL_ACTIONS_CALL_HOME        = 0x0800, // Call Home Required
    ERRL_ACTIONS_FNM_REQ          = 0x0400, // Further Analysis Required
    ERRL_ACTIONS_SP_CALL_HOME     = 0x0100, // The service processor should call home

    // The Following are internal actions and should NOT
    // be used by callers unless explicitly allowed.
    ERRL_ACTIONS_SKIP_ACK                    = 0x0001, // Override Error Logger's default ack wait
    ERRL_ACTIONS_DONT_TERMINATE              = 0x0002, // Override Error Logger's termination call
    ERRL_ACTIONS_IGNORE_CONSISTENCY_CHECKS   = 0x0004, // Error Logging infrastrucure will ignore consistency checking on this error log
};

// Severity Categories
// Corresponds to the first nibble of the error log severity definition
enum errlSevCat
{
    ERRL_SEVCAT_INFORMATIONAL  = 0,
    ERRL_SEVCAT_RECOVERED      = 1,
    ERRL_SEVCAT_PREDICTIVE     = 2,
    ERRL_SEVCAT_UNRECOVERABLE  = 4,
    ERRL_SEVCAT_CRITICAL       = 5,
    ERRL_SEVCAT_DIAGNOSTIC     = 6,
    ERRL_SEVCAT_SYMPTOM        = 7
};

/**
 *  @brief Creator types.
 */
enum errlCreator_t
{
    ERRL_CID_HOSTBOOT                       = 'B',
    ERRL_CID_EBMC                           = 'O',
};




/**
 * @brief Enumeration of PEL section eyecatchers. These are
 * usually handled as a uint16_t.  See
 * "eCLipz and P7 Platform Event Log and SRC PLDD" documentation
 * https://mcdoc.boeblingen.de.ibm.com/out/out.ViewDocument.php?documentid=1675
 */
enum errlSectionId_t
{
    ERRL_SID_PRIVATE_HEADER                 = 0x5048, // PH private header
    ERRL_SID_USER_HEADER                    = 0x5548, // UH user header
    ERRL_SID_EXTENDED_HEADER                = 0x4548, // EH user header
    ERRL_SID_PRIMARY_SRC                    = 0x5053, // PS
    ERRL_SID_USER_DEFINED                   = 0x5544, // UD user defined
    ERRL_SID_EXTENDED_USER_DEFINED          = 0x4544, // ED user defined
    ERRL_SID_FAILING_ENCLOSURE_MTMS         = 0x4D54, // MT user defined
};


/**
 * @brief Enumeration of error log severity.
 * Needs to fit into 1 byte for flattening purposes.
 *
 * Severity resides in the user header of the PEL log.
 *
 * Refer to this ErrlEntry setter:
 * void ErrlEntry::setSev(const errlSeverity_t i_sev)
 *
 * NOTE: If any values are added, please update std::map errl_sev_str_map
 *       in errlentry.C
 */
enum errlSeverity_t
{
    ERRL_SEV_INFORMATIONAL                     = 0x00,     // Informational Error, general
    ERRL_SEV_RECOVERED                         = 0x10,     // Recovered Error, general
    ERRL_SEV_PREDICTIVE                        = 0x20,     // Predictive Error, general
    ERRL_SEV_PREDICTIVE_DEGRADED               = 0x21,     // Predictive, degraded performance
    ERRL_SEV_PREDICTIVE_CORRECTABLE            = 0x22,     // Predictive, may correct after re-ipl
    ERRL_SEV_PREDICTIVE_CORRECTABLE2           = 0x23,     // Predictive, degraded perf, re-ipl correctable
    ERRL_SEV_PREDICTIVE_REDUNDANCY_LOST        = 0x24,     // Predictive, loss of redundancy
    ERRL_SEV_UNRECOVERABLE                     = 0x40,     // Unrecoverable, general
    ERRL_SEV_UNRECOVERABLE1                    = 0x41,     // Unrecoverable, bypassed w/degraded perf
    ERRL_SEV_UNRECOVERABLE2                    = 0x44,     // Unrecoverable, bypassed w/loss of redundancy
    ERRL_SEV_UNRECOVERABLE3                    = 0x45,     // Unrecoverable, bypassed w/loss of red+perf
    ERRL_SEV_UNRECOVERABLE4                    = 0x48,     // Unrecoverable, bypassed w/loss of function
    ERRL_SEV_CRITICAL_FAIL_UNKNOWN             = 0x50,     // Critical, Scope of failure unknown
    ERRL_SEV_CRITICAL_SYS_TERM                 = 0x51,     // Critical, System Termination
    ERRL_SEV_CRITICAL_SYS_FAIL                 = 0x52,     // Critical, System Failure
    ERRL_SEV_CRITICAL_PART_TERM                = 0x53,     // Critical, Partition Termination
    ERRL_SEV_CRITICAL_PART_FAIL                = 0x54,     // Critical, Partition Failure
    ERRL_SEV_DIAGNOSTIC_ERROR1                 = 0x60,     // Disagnostic test error
    ERRL_SEV_DIAGNOSTIC_ERROR2                 = 0x61,     // Diagnostic test error, resource may produce bad results
    ERRL_SEV_SYMPTOM_RECOVERED                 = 0x71,     // Symptom Recovered
    ERRL_SEV_SYMPTOM_PREDICTIVE                = 0x72,     // Symptom Predictive
    ERRL_SEV_SYMPTOM_UNRECOV                   = 0x74,     // Symptom Unrecoverable
    ERRL_SEV_SYMPTOM_DIAGERR                   = 0x76,     // Symptom Diag Err
    ERRL_SEV_UNKNOWN                           = 0xFF      // Reserved
};




/**
 * @brief Event (error) type
 * Needs to fit into 1 byte for flattening purposes.
 *
 * Event type resides in user header of PEL log.
 *
 * Refer to this ErrlEntry setter:
 * void ErrlEntry::setEventType(const errlEventType_t i_eventType)
 */
enum errlEventType_t
{
    ERRL_ETYPE_NOT_APPLICABLE           = 0x00,
    ERRL_ETYPE_MISCELLANEOUS            = 0x01,
    ERRL_ETYPE_TRACING                  = 0x02,
    ERRL_ETYPE_DUMP_NOTIFICATION        = 0x08,
    ERRL_ETYPE_USER_DECONFIG            = 0x20,
    ERRL_ETYPE_SYS_DECONFIG             = 0x21,
    ERRL_ETYPE_CONCURRENT_MAINTENANCE   = 0x40,
    ERRL_ETYPE_CAPACITY_UPGRADE         = 0x60,
};




/**
 * @brief SRC type definitions
 * Needs to fit into 1 byte for flattening purposes.
 *
 * This represents the byte in an SRC in position
 * XX......  such as BC in BC81F00B.
 *
 * Refer to errlentry.H and this setter:
 * void ErrlEntry::setSrcType(const srcType_t i_srcType)
 *
 */
enum srcType_t
{
    SRC_INTERVENTION_REQ   = 0xAC,
    SRC_ERR_INFO           = 0xBC,
    SRC_IPL_STATUS         = 0xCC,
    SRC_GENERAL_STATUS     = 0xDC,
};


/**
 * @brief Sub system definitions
 * Needs to fit into 1 byte for flattening purposes.
 * This represents the byte in an SRC in position
 * ..XX....  such as 81 in B181F00B.
 *
 * Refer to errentry.H and src/epub/fsp/epub_service_codes.H
 * in fips code.
 *
 * @note This enum is parsed by genErrlParsers.pl and requires the subsystem
 *       name to start with EPUB_  and the value to start with 0x followed by
 *       exactly two hex digits and a comma. Subsystems that have their SRCs
 *       published separately (e.g. PRD) are marked as //@PUB_IGNORE
 */
enum epubSubSystem_t
{
    EPUB_RESERVED_0                 = 0x00, //@PUB_IGNORE

    // Processor subsystem
    EPUB_PROCESSOR_SUBSYS           = 0x10,
    EPUB_PROCESSOR_FRU              = 0x11,
    EPUB_PROCESSOR_CHIP_CACHE       = 0x12,
    EPUB_PROCESSOR_UNIT             = 0x13,
    EPUB_PROCESSOR_BUS_CTL          = 0x14,

    // Memory subsystem
    EPUB_MEMORY_SUBSYS              = 0x20,
    EPUB_MEMORY_CONTROLLER          = 0x21,
    EPUB_MEMORY_BUS                 = 0x22,
    EPUB_MEMORY_DIMM                = 0x23,
    EPUB_MEMORY_FRU                 = 0x24,
    EPUB_EXTERNAL_CACHE             = 0x25,

    // I/O subsystem: hub and bridge
    EPUB_IO_SUBSYS                  = 0x30,
    EPUB_IO_PHB                     = 0x38,

    // CEC Hardware
    EPUB_CEC_HDW_SUBSYS             = 0x50,
    EPUB_CEC_HDW_VPD_INTF           = 0x55,
    EPUB_CEC_HDW_I2C_DEVS           = 0x56,
    EPUB_CEC_HDW_CHIP_INTF          = 0x57, // includes JTAG, FSI, etc.
    EPUB_CEC_HDW_CLK_CTL            = 0x58,
    EPUB_CEC_HDW_TOD_HDW            = 0x5A,
    EPUB_CEC_HDW_SP_PHYP_INTF       = 0x5C,

    // Power/Cooling subsystem
    EPUB_POWER_SUBSYS               = 0x60,

    // Others
    EPUB_MISC_SUBSYS                = 0x70,
    EPUB_MISC_TEST_TOOL             = 0x72, //@PUB_IGNORE
    EPUB_MISC_MULTIPLE_SUBSYS       = 0x74, //@PUB_IGNORE
    EPUB_MISC_UNKNOWN               = 0x75, //@PUB_IGNORE
    EPUB_MISC_INFORMATIONAL         = 0x76, //@PUB_IGNORE

    // Platform Firmware
    EPUB_FIRMWARE_SUBSYS            = 0x80,
    EPUB_FIRMWARE_SP                = 0x81,
    EPUB_FIRMWARE_PHYP              = 0x82,

    EPUB_FIRMWARE_HOSTBOOT          = 0x8A,
    EPUB_UNKNOWN                    = 0xFF,
};



/**
 * @brief Terminating flag definitions
 * Needs to fit into 1 word (32-bits) for flattening purposes.
 */
enum errlTermState_t
{
    TERM_STATE_NO_FLAGS        = 0x00000000,
    TERM_STATE_MNFG            = 0x10000000,
    TERM_STATE_SOFT            = 0x20000000,
    TERM_STATE_UNKNOWN         = 0xFFFFFFFF,
};




/** @enum  errlUserDataType_t
  *
  * These are section type identifiers for the ERRL component for
  * user-defined data sections. For example, if somebody calls
  *           errl->collectTrace("INITSVC")
  * then the resulting UD PEL section header will be tagged with
  *     component id:       ERRL_COMP_ID
  *     section type:       ERRL_UDT_TRACE
  *     version:            ERRL_UDV_DEFAULT_VER_1
  *
  * Other components will have to code a similar enum so that
  * the the errl parser/pretty printer can "dispatch" a user-defined
  * PEL section to them for printing based on the three values in
  * the UD PEL section: component id, section type, and version.
  *
  * The section type field in the PEL header is one byte long.
  */
enum errlUserDataType_t
{
    FIPS_ERRL_UDT_HB_TRACE          = 0x15,   // A trace buffer
};


/** @enum  errlUserDataVersion
  *
  * Identifier for the version of user data in an error log.
  * This value is used in conjunction with the type field to format
  * of the additional data in an error log.  Versions and type should
  * be unique across subcomponents of the errl component.
*/
enum errlUserDataVersion
{
    FIPS_ERRL_UDV_DEFAULT_VER_1      = 0x0001,   // Version 1, default info
};


/**
 * @brief  This is the size of the storage buffer, presently
 * in L3 RAM.  When this storage is filled, no more error logs
 * will be committed. The L3 RAM storage is g_ErrlStorage[]
 * declared in errlmanager.C in BSS data. This will change
 * when switching to PNOR.
 */
const uint32_t ERRL_STORAGE_SIZE = 65536;


/**
 * @brief  This is the number of additional error logs available to store
 * additional error log data (usually traces), beyond the original error log.
 */
const uint32_t MAX_EXTRA_LOGS_PER_ORIGINAL = 5;

/**
 *
 *  @typedef    version number
 *      version number of the user data
 */
typedef uint8_t errlver_t;

/**
 * @typedef errlsubsec_t
 *      errorlog subsection id of the user data
 */
typedef uint8_t errlsubsec_t;


/**

 * @brief The RAM storage for committed error logs starts with this
 * header.  This structure is org'ed at &g_ErrlStorage[0].  All
 * offsets are based from &g_achErrlStorage[0], so the first
 * marker will appear at offset sizeof( storage_header_t )
 */
typedef struct storage_header
{
    uint32_t    cbStorage;        // count of bytes in buffer
    uint32_t    cInserted;        // count of logs ever committed
    uint32_t    offsetStart;      // start-of-list marker
    uint32_t    offsetMarker;     // next-insertion marker
} __attribute__((packed)) storage_header_t;




/**
 * @brief The storage buffer is a series of flattened error logs
 * interspersed with markers.
 * A typical marker will give the offset to the next marker and also
 * the length of the flattened error log data.
 */
typedef struct marker
{
    uint32_t    offsetNext;
    uint32_t    length;
} __attribute__((packed)) marker_t;






/**
 * @brief pelSectionHeader_t
 *
 * A PEL structure used to flatten error logs to PEL format.
 *
 * This structure represents the first 8 bytes of any PEL section.
 * Populated by the flatten code of class ErrlSctnHdr for any PEL
 * section.  Classes ErrlUH, ErrlED, ErrlPrvt, and ErrlSctn each have a
 * ErrlSctnHdr.  User-defined sections derive from ErrlSctn, so
 * their section header works though ErrlSctn parent class.
 *
 * When parsing a PEL data set, start at offset 0 (the
 * first section).  Then add "len" to know the starting offset
 * of the second section, and so on.
 *
 * When pretty-printing a user-defined PEL section, the
 * controlling logic will "dispatch" a PEL section to a
 * user-supplied printing function based on the component ID,
 * section type, and version.
 */
typedef struct pelsectionheader
{
  uint16_t   sid;     // section identifier
  uint16_t   len;     // overall length of this section
  uint8_t    ver;     // section version
  uint8_t    sst;     // section type
  uint16_t   compId;  // component ID
} __attribute__((packed))  pelSectionHeader_t;



/**
 * @brief pelPrivateHeaderSection_t
 *
 * A PEL structure used to flatten error logs to PEL format.
 *
 * This structure represents an entire Private Header (PH)
 * PEL section. This section is a fixed overall
 * length of 48 bytes including the PEL section header.
 * This is the first section found in a flat PEL dataset.
 * Populated by the flatten code of class ErrlPrvt.
 *
 * The component named in the section header is the
 * creating component of the error log.
 */
typedef struct pelprivateheadersection
{
  pelSectionHeader_t  sectionheader;
  uint64_t   creationTime;
  uint64_t   commitTime;
  uint8_t    creatorId;   // 'B' for Hostboot
  uint8_t    reserved0;
  uint8_t    reserved1;
  uint8_t    sectionCount;
  uint32_t   reserved2;
  uint64_t   creatorImplementation;
  uint32_t   plid;
  uint32_t   eid;
} __attribute__((packed)) pelPrivateHeaderSection_t;

/**
 * @brief pelEDHeaderSection_t
 *
 * A PEL structure used to flatten error logs to PEL format.
 *
 * This structure represents an entire Extended User
 * Defined Data (ED) PEL section. This section is a fixed overall
 * total length of 212 bytes including the PEL section header.
 * Populated by the flatten code of class ErrlED.
 *
 * The creator named is EBMC.
 *
 * See eBMC API makeSysInfoUserDataSection
 * https://github.com/openbmc/phosphor-logging/blob/master/extensions/openpower-pels/pel.cpp
 *
 * eBMC will populate the ED section when the log is sent down to the eBMC.
 * The pel.cpp is the authoritative source, examples are process name, fw version and states.
 */
typedef struct pelEDheadersection
{
  pelSectionHeader_t  sectionheader = { };
  uint8_t    creatorId = ERRL_CID_EBMC; // 'O' for EBMC
  char reserved1[3] = { };
  char ed_data[200] = { };
  // length 212 total
} __attribute__((packed)) pelEDHeaderSection_t;

/**
 * @brief pelUserHeaderSection_t
 *
 * A PEL structure used to flatten error logs to PEL format.
 *
 * This structure represents an entire User Header (UH)
 * PEL section. This section is a fixed overall
 * length of 0x18 bytes including the PEL section header.
 * This is the second section found in a flat PEL dataset.
 * Populated by the flatten code of class ErrlUH.
 *
 * The component named in the section header is the
 * committing component of the error log.
 */
typedef struct peluserheadersection
{
  pelSectionHeader_t sectionheader;
  uint8_t    ssid;   // subsystem id
  uint8_t    scope;  // event scope
  uint8_t    sev;    // severity
  uint8_t    etype;  // event type
  uint32_t   reserved0;  // for error log return code
  uint8_t    domain;     // not used in Hostboot
  uint8_t    vector;     // not used in Hostboot
  uint16_t   actions;    // TODO RTC 93631
  uint32_t   reserved1;
} __attribute__((packed)) pelUserHeaderSection_t;



/**
 * @brief Structure to represent machine type/model and serial number, as used
 * in the EH and MT sections below
 */
typedef struct mtms_t
{
    char mtm[8] = { }; // Machine type/model string
    char serial[12] = { };
} __attribute__((packed)) mtms_t;



/** @brief Function to set the value of errl string variables.
 *  @param[out] o_array Array to set
 *  @param[in] i_value  Value to assign to array
 *  @param[in] i_nullTerm Add null terminator to end of string
 *
 *  This function will zero out the given array and set its contents to the
 *  given value, without writing beyond the bounds of the array. If the given
 *  value is too long, it will be truncated. The string in the output array
 *  will be zero-terminated if nullTerm is true.
 */
template<uint64_t Size>
uint64_t set_errl_string(char(&o_array)[Size],
                         const char* const i_value,
                         bool i_nullTerm = true)
{
    uint64_t value_len = 0;
    if (i_nullTerm)
    {
        value_len = std::min(strlen(i_value), Size - 1);
    }
    else
    {
        value_len = std::min(strlen(i_value), Size);
    }
    memset(o_array, 0, Size);
    memcpy(o_array, i_value, value_len);
    return value_len;
}



#ifdef CONFIG_BUILD_FULL_PEL

// These are the max string sizes defined in the PEL spec. They may not match
// the attribute sizes they are copied from so are truncated if necessary by
// the set_errl_string function.
// PEL spec 4.2.2.2
constexpr size_t PEL_LOC_CODE_SIZE = 80;
// PEL spec 4.2.2.2.3
constexpr size_t PEL_PART_NUM_SIZE = 8;
constexpr size_t PEL_CCIN_SIZE = 4;
constexpr size_t PEL_SERIAL_NUM_SIZE = 12;

/**
 * @brief PEL SRC subsection header
 *        PEL spec 4.2.2
 */
typedef struct pelsrcsubsectionheader
{
  uint8_t    ssid;    // subsection identifier
  uint8_t    ssflags; // subsection flags
  uint16_t   sslen;   // overall length of this subsection
} __attribute__((packed))  pelSubSectionHeader_t;  // 4 bytes


/**
 * @brief SRC FRU callout header
 *        PEL spec 4.2.2.2
 */
typedef struct pelfrucalloutheader
{
  uint8_t    fclen;    // fru callout length
  uint8_t    fcflags;  // fru callout type and flags
  uint8_t    fcpri;    // fru callout priority
  uint8_t    fclclen;  // fru callout location code length
} __attribute__((packed))  pelFRUCalloutHeader_t;  // 4 bytes


/**
 * @brief SRC FRU substruct header
 *        PEL spec 4.2.2.2.2
 */
typedef struct pelfrusubstructheader
{
  uint16_t   fsstype;
  uint8_t    fsslen;
  uint8_t    fssflags;
} __attribute__((packed))  pelFRUSubstructHeader_t;  // 4 bytes


/**
 * @brief SRC FRU ID substruct
 *        PEL spec 4.2.2.2.3
 */
typedef struct pelfruidsubstruct
{
  pelFRUSubstructHeader_t frusshead; // 4 bytes
  char      fruidpnString[PEL_PART_NUM_SIZE]; // present if flags[4] or flags[6]
  char      fruidccinString[PEL_CCIN_SIZE];   // present if flags[5]
  char      fruidsnString[PEL_SERIAL_NUM_SIZE];  // present if flags[7]
} __attribute__((packed))  pelFRUIDSubstruct_t; // 28 bytes


/**
 * @brief SRC PCE ID substruct
 *        PEL spec 4.2.2.2.4
 * Placeholder, not currently used
 */
//typedef struct pelpceidsubstruct
//{
//  pelFRUSubstructHeader_t frusshead;
//  mtms_t     pceidmtms;    // string 20
//  //char       pceidnameString[32];  // variable up to 32
//} __attribute__((packed))  pelPCEIDSubstruct_t;


/**
 * @brief SRC MRU field entry
 *        PEL spec 4.2.2.2.6
 * Placeholder, not currently used
 */
//typedef struct mruidfield
//{
//  uint32_t       mrufieldreserved:24;
//  char           mrureppri;
//  uint32_t       mruid;
//} __attribute__((packed))  pelMRUIDField_t; // 8 bytes


/**
 * @brief SRC MRU ID substruct
 *        PEL spec 4.2.2.2.5
 * Placeholder, not currently used
 */
//typedef struct pelmruidsubstruct
//{
//  pelFRUSubstructHeader_t frusshead; // 4 bytes
//  uint32_t   mrussreserved;
//} __attribute__((packed))  pelMRUIDSubstruct_t; // 8 bytes

// Flags for FRU Identity Substructure 4.2.2.2.3
// Used to identify the type of FRU callout and what info is contained. Missing values below are
// considered reserved for future expansion. Explicitly reserved values are represented below.
//
// Bits 0-3 are the failing component type. Only one can be selected.
// Bits 4-7 are which information was supplied with the callout and can be bitwised together to
//          indicate multiple things were supplied.
//
// For more verbose descriptions, refer to PEL spec section 4.2.2.2.3.
enum fruIdentitySubstructFlags : uint8_t
{
    // Failing Component Type bits
    // Reserved
    FAILING_COMP_TYPE_RESERVED_MIN_VAL = 0x00,
    // Hardware FRU in platform
    FAILING_COMP_TYPE_NORMAL_HW        = 0x10,
    // Some layer of platform firmware or OS code
    FAILING_COMP_TYPE_CODE             = 0x20,
    // Configuration error, could be related to how hardware or code is configured.
    FAILING_COMP_TYPE_CNFG_ERR         = 0x30,
    // Maintenance procedure required. Further isolation required by performing procedure as IDed by Procedure ID field.
    FAILING_COMP_TYPE_MAINT            = 0x40,
    // Failing part is not within system, e.g. attached storage, external drives, etc.
    FAILING_COMP_TYPE_EXTERN_FRU       = 0x90,
    // Failing code is not running in the platform. Could be something like storage subsystem code, etc.
    FAILING_COMP_TYPE_EXTERN_CODE      = 0xA0,
    // A special tool required by one of the FRUs in the list contained within an error log.
    FAILING_COMP_TYPE_TOOL_FRU         = 0xB0,
    // Analysis code knows exactly which part is failing but there is no part number for various reasons.
    FAILING_COMP_TYPE_SYMBOLIC_FRU     = 0xC0,
    // Similar to symbolic fru type except a trusted location code is provided. Can only be used when symbolic FRUs
    // represent hardware and when part number can not be determined.
    FAILING_COMP_TYPE_SYMBOLIC_FRU_LOC = 0xE0,
    // Reserved
    FAILING_COMP_TYPE_RESERVED_MAX_VAL = 0xF0,

};

// Information supplied bits 4-7. See fruIdentitySubstructFlags for more info.
enum fruIdentitySubstructInfo : uint8_t
{

    // FRU stocking part number supplied, mutually exclusive with bit 6 FAILING_COMP_TYPE_FRU_PRC.
    FAILING_COMP_TYPE_FRU_PN   = 0x08,
    // FRU CCIN supplied. Only valid if bit 4 FAILING_COMP_TYPE_FRU_PN is set.
    FAILING_COMP_TYPE_FRU_CCIN = 0x04,
    // Maintenance procedure callout supplied, mutually exclusive with bit 4 FAILING_COMP_TYPE_FRU_PN.
    FAILING_COMP_TYPE_FRU_PRC  = 0x02,
    // FRU Serial Number supplied. Only valid if bit 4 FAILING_COMP_TYPE_FRU_PN is set.
    FAILING_COMP_TYPE_FRU_SN   = 0x01,
};

/**
 * @brief PEL SRC subsection
 *        PEL spec 4.2.2.1 and 4.2.2.2
 *        Listed here for reference only
 */
  // pelSubSectionHeader_t ssheader;    // only 1 subsection header
  //   pelFRUCalloutHeader_t frucoheader;    // 1 to 10 FRU callouts
  //   char                  locationcode[]; // variable up to 80
  //   pelFRUIDSubstruct_t   fruss;          // variable up to 28
  //   pelPCEIDSubstruct_t   pcess;          // variable up to 56
  //   pelMRUSubstruct_t     mruss;          // variable up to 128

/**
 * @brief Entry struct for the SRC FRU callout vector
 *        Internal-only structure that contains FRU data saved during commit().
 *        Added to error log in flatten() using PEL spec structures above.
 */
typedef struct frucoentry
{
    uint32_t        priority = 0;
    size_t          locCodeLen = 0;
    char            locationCode[PEL_LOC_CODE_SIZE] = {0};
    uint8_t         fruCompType = 0;
    char            partNumber[PEL_PART_NUM_SIZE] = {0};
    char            ccin[PEL_CCIN_SIZE] = {0};
    char            serialNumber[PEL_SERIAL_NUM_SIZE] = {0};
} fruCallOutEntry_t;

#endif

constexpr size_t SRC_STRING_SIZE = 32;

/**
 * @brief PEL SRC Flags
 *        PEL spec 4.2
 *
 */
typedef struct pelsrcflags
{
  uint8_t   srcNotPostedToOpPanel:1;        // bit 0
  uint8_t   reserved:2;                     // bit 1-2
  uint8_t   srcIsServiceable:1;             // bit 3
  uint8_t   srcIsPostedToOpPanel:1;         // bit 4
  uint8_t   hypDumpInitiated:1;             // bit 5
  uint8_t   spcnPowerFault:1;               // bit 6
  uint8_t   additionalSubsection:1;         // bit 7
} __attribute__((packed)) pelSRCFlags_t;

/**
 * @brief SRC section definition
 *
 * Hostboot generates a minimal SRC as described under
 * "Common Usage of the SRC Structure" (p.69) in the PEL
 * reference (mcdoc 1675).
 */
struct SrcSection
{
  uint8_t       ver;        // src version, usually a 2
  pelSRCFlags_t flags;      // src flags
  uint8_t       reserved0;
  uint8_t       wordcount;  // usually 9
  uint16_t      reserved1;  // usually 0, but I put reasonCode here
  uint16_t      srcLength;  // usually 72
  // "Hex word 2"
  // Low byte of this word is SRC format 0xE0.
  uint32_t      word2;
  // "Hex word 3"
  uint16_t      word3;
  uint8_t       moduleId;
  uint8_t       reserved2;
  // "Hex word 4" last progress code
  uint32_t      word4;
  // "Hex word 5" error status flags, etc.
  uint32_t      word5;
  // "Hex word 6-7" optional, iv_user1
  uint64_t      word6;
  // "Hex word 8-9" optional, iv_user2
  uint64_t      word8;
  char          srcString[SRC_STRING_SIZE];
} __attribute__((packed));

/**
 * @brief pelSRCSection_t
 *
 * A PEL structure used to flatten error logs to PEL format.
 *
 * This structure represents an entire Primary SRC (PS)
 * PEL section. This section is a fixed overall
 * length of 80 bytes including the PEL section header.
 * This is the usually the third section found in a flat PEL dataset.
 * Populated by the flatten code of class ErrlSRC.
 *
 * Hostboot generates a minimal SRC as described under
 * "Common Usage of the SRC Structure" (p.69) in the PEL
 * reference (mcdoc 1675). The page numbers below refer
 * to version 0.8 of that document.
 */
typedef struct pelsrcsection
{
  pelSectionHeader_t sectionheader;
  SrcSection src;
} __attribute__((packed)) pelSRCSection_t;


/**
 * @brief pelExtendedUserHeaderSection_t
 *
 * A PEL structure used to flatten error logs to PEL format.
 *
 * This structure represents an entire Extended User Header (EH) PEL
 * section. This section is a variable overall length.  This is the fourth section
 * found in a flat PEL dataset.  Populated by the flatten code of class ErrlEH.
 *
 * Note that while the symptom_id may be a maximum of 80 characters, it may be
 * less in practice, and the array should not be addressed beyond the number of
 * characters indicated by symptom_id_length (this includes by indirect copying
 * such as assigning the structure, passing it as an argument by value, etc).
 *
 * The component named in the section header is the committing component of the
 * error log.
 */
typedef struct pelextendeduserheadersection
{
    pelSectionHeader_t sectionheader = { };
    mtms_t mtms = { };
    char firmware[16] = { }; // server firmware released version
    char subsystem[16] = { }; // firmware subsystem driver version
    uint32_t reserved = 0;
    uint64_t timestamp = 0;
    char reserved2[3] = { };
    uint8_t symptom_id_length = 0;
    char symptom_id[80] = { };
} __attribute__((packed)) pelExtendedUserHeaderSection_t;

/**
 * @brief pelFailingEnclosureMtmsSection_t
 *
 * A PEL structure used to flatten error logs to PEL format.
 *
 * This structure represents an entire Failing Enclosure MTMS (MT) PEL
 * section. Populated by the flatten code of class ErrlMT.
 *
 * The component named in the section header is the committing component of the
 * error log.
 */
typedef struct pelfailingenclosuremtmssection
{
    pelSectionHeader_t sectionheader = { };
    mtms_t mtms = { };
} __attribute__((packed)) pelFailingEnclosureMtmsSection_t;


} // End namespace

#endif // HBERRLTYPES_H
