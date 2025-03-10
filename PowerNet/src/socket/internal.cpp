#include "internal.hpp"
#include "MacErrors.h"
#include "MacMemory.h"
#include "MacTypes.h"
#include "Memory.h"
#include "OpenTransport.h"
#include "Threads.h"
#include "internal.hpp"
#include <cstdarg>
#include <cstdlib>

extern ThreadID read_thread_id;

void __throw_os_err(const char *file, int line, const char *func, OSErr err) {
  switch (err) {
  case kOTOutOfMemoryErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTOutOfMemoryErr",
                    func, file, line);
    return;
  case kOTNotFoundErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTNotFoundErr",
                    func, file, line);
    return;
  case kOTDuplicateFoundErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTDuplicateFoundErr",
                    func, file, line);
    return;
  case kOTBadAddressErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTBadAddressErr",
                    func, file, line);
    return;
  case kOTBadOptionErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTBadOptionErr",
                    func, file, line);
    return;
  case kOTAccessErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kOTAccessErr",
        func, file, line);
    return;
  case kOTBadReferenceErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTBadReferenceErr",
                    func, file, line);
    return;
  case kOTNoAddressErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTNoAddressErr",
                    func, file, line);
    return;
  case kOTOutStateErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTOutStateErr",
                    func, file, line);
    return;
  case kOTBadSequenceErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTBadSequenceErr",
                    func, file, line);
    return;
  case kOTSysErrorErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTSysErrorErr",
                    func, file, line);
    return;
  case kOTLookErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kOTLookErr",
        func, file, line);
    return;
  case kOTBadDataErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTBadDataErr",
                    func, file, line);
    return;
  case kOTBufferOverflowErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTBufferOverflowErr",
                    func, file, line);
    return;
  case kOTFlowErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kOTFlowErr",
        func, file, line);
    return;
  case kOTNoDataErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kOTNoDataErr",
        func, file, line);
    return;
  case kOTNoDisconnectErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTNoDisconnectErr",
                    func, file, line);
    return;
  case kOTNoUDErrErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTNoUDErrErr",
                    func, file, line);
    return;
  case kOTBadFlagErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTBadFlagErr",
                    func, file, line);
    return;
  case kOTNoReleaseErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTNoReleaseErr",
                    func, file, line);
    return;
  case kOTNotSupportedErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTNotSupportedErr",
                    func, file, line);
    return;
  case kOTStateChangeErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTStateChangeErr",
                    func, file, line);
    return;
  case kOTNoStructureTypeErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTNoStructureTypeErr",
                    func, file, line);
    return;
  case kOTBadNameErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTBadNameErr",
                    func, file, line);
    return;
  case kOTBadQLenErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTBadQLenErr",
                    func, file, line);
    return;
  case kOTAddressBusyErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTAddressBusyErr",
                    func, file, line);
    return;
  case kOTIndOutErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kOTIndOutErr",
        func, file, line);
    return;
  case kOTProviderMismatchErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTProviderMismatchErr",
                    func, file, line);
    return;
  case kOTResQLenErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTResQLenErr",
                    func, file, line);
    return;
  case kOTResAddressErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTResAddressErr",
                    func, file, line);
    return;
  case kOTQFullErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kOTQFullErr",
        func, file, line);
    return;
  case kOTProtocolErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTProtocolErr",
                    func, file, line);
    return;
  case kOTBadSyncErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTBadSyncErr",
                    func, file, line);
    return;
  case kOTCanceledErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTCanceledErr",
                    func, file, line);
    return;
  case kEPERMErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kEPERMErr",
        func, file, line);
    return;
  case kENORSRCErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kENORSRCErr",
        func, file, line);
    return;
  case kEINTRErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kEINTRErr",
        func, file, line);
    return;
  case kEIOErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kEIOErr", func,
        file, line);
    return;
  case kENXIOErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kENXIOErr",
        func, file, line);
    return;
  case kEBADFErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kEBADFErr",
        func, file, line);
    return;
  case kEAGAINErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kEAGAINErr",
        func, file, line);
    return;
  case kEACCESErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kEACCESErr",
        func, file, line);
    return;
  case kEFAULTErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kEFAULTErr",
        func, file, line);
    return;
  case kEBUSYErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kEBUSYErr",
        func, file, line);
    return;
  case kENODEVErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kENODEVErr",
        func, file, line);
    return;
  case kEINVALErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kEINVALErr",
        func, file, line);
    return;
  case kENOTTYErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kENOTTYErr",
        func, file, line);
    return;
  case kEPIPEErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kEPIPEErr",
        func, file, line);
    return;
  case kERANGEErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kERANGEErr",
        func, file, line);
    return;
  case kEWOULDBLOCKErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kEWOULDBLOCKErr or kEDEADLKErr",
                    func, file, line);
    return;
  case kEALREADYErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kEALREADYErr",
        func, file, line);
    return;
  case kENOTSOCKErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kENOTSOCKErr",
        func, file, line);
    return;
  case kEDESTADDRREQErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kEDESTADDRREQErr",
                    func, file, line);
    return;
  case kEMSGSIZEErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kEMSGSIZEErr",
        func, file, line);
    return;
  case kEPROTOTYPEErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kEPROTOTYPEErr",
                    func, file, line);
    return;
  case kENOPROTOOPTErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kENOPROTOOPTErr",
                    func, file, line);
    return;
  case kEPROTONOSUPPORTErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kEPROTONOSUPPORTErr",
                    func, file, line);
    return;
  case kESOCKTNOSUPPORTErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kESOCKTNOSUPPORTErr",
                    func, file, line);
    return;
  case kEOPNOTSUPPErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kEOPNOTSUPPErr",
                    func, file, line);
    return;
  case kEADDRINUSEErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kEADDRINUSEErr",
                    func, file, line);
    return;
  case kEADDRNOTAVAILErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kEADDRNOTAVAILErr",
                    func, file, line);
    return;
  case kENETDOWNErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kENETDOWNErr",
        func, file, line);
    return;
  case kENETUNREACHErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kENETUNREACHErr",
                    func, file, line);
    return;
  case kENETRESETErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kENETRESETErr",
                    func, file, line);
    return;
  case kECONNABORTEDErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kECONNABORTEDErr",
                    func, file, line);
    return;
  case kECONNRESETErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kECONNRESETErr",
                    func, file, line);
    return;
  case kENOBUFSErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kENOBUFSErr",
        func, file, line);
    return;
  case kEISCONNErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kEISCONNErr",
        func, file, line);
    return;
  case kENOTCONNErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kENOTCONNErr",
        func, file, line);
    return;
  case kESHUTDOWNErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kESHUTDOWNErr",
                    func, file, line);
    return;
  case kETOOMANYREFSErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kETOOMANYREFSErr",
                    func, file, line);
    return;
  case kETIMEDOUTErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kETIMEDOUTErr",
                    func, file, line);
    return;
  case kECONNREFUSEDErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kECONNREFUSEDErr",
                    func, file, line);
    return;
  case kEHOSTDOWNErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kEHOSTDOWNErr",
                    func, file, line);
    return;
  case kEHOSTUNREACHErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kEHOSTUNREACHErr",
                    func, file, line);
    return;
  case kEPROTOErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kEPROTOErr",
        func, file, line);
    return;
  case kETIMEErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kETIMEErr",
        func, file, line);
    return;
  case kENOSRErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kENOSRErr",
        func, file, line);
    return;
  case kEBADMSGErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kEBADMSGErr",
        func, file, line);
    return;
  case kECANCELErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kECANCELErr",
        func, file, line);
    return;
  case kENOSTRErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kENOSTRErr",
        func, file, line);
    return;
  case kENODATAErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kENODATAErr",
        func, file, line);
    return;
  case kEINPROGRESSErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kEINPROGRESSErr",
                    func, file, line);
    return;
  case kESRCHErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kESRCHErr",
        func, file, line);
    return;
  case kENOMSGErr:
    mac_error_throw(
        "Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: kENOMSGErr",
        func, file, line);
    return;
  case kOTClientNotInittedErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTClientNotInittedErr",
                    func, file, line);
    return;
  case kOTPortHasDiedErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTPortHasDiedErr",
                    func, file, line);
    return;
  case kOTPortWasEjectedErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTPortWasEjectedErr",
                    func, file, line);
    return;
  case kOTBadConfigurationErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTBadConfigurationErr",
                    func, file, line);
    return;
  case kOTConfigurationChangedErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTConfigurationChangedErr",
                    func, file, line);
    return;
  case kOTUserRequestedErr:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTUserRequestedErr",
                    func, file, line);
    return;
  case kOTPortLostConnection:
    mac_error_throw("Error:\n\t- Function:%s\n\t- Position:%s:%d\n\t- Error: "
                    "kOTPortLostConnection",
                    func, file, line);
    return;
  }
}

Socket::~Socket() { OTCloseProvider(endpoint); }

std::unordered_map<size_t, Socket *> openSockets =
    std::unordered_map<size_t, Socket *>();

void mac_error_throw(const char *format, ...) {
  va_list arglist;

  va_start(arglist, format);

  char *buf = (char *)malloc(255);
  vsnprintf(buf, 255, format, arglist);

  va_end(arglist);

  printf("%s\n", buf);

  void *what;
  DisposeThread(read_thread_id, NULL, false);
  DisposeThread(main_thread_id, NULL, false);
  YieldToAnyThread();
}

std::vector<uint8_t> flatten(const std::vector<std::vector<uint8_t>> &v) {
  std::size_t total_size = 0;
  for (const auto &sub : v)
    total_size += sub.size(); // I wish there was a transform_accumulate
  std::vector<uint8_t> result;
  result.reserve(total_size);
  for (const auto &sub : v)
    result.insert(result.end(), sub.begin(), sub.end());
  return result;
}

int __mem_test(const char *file, int line, void *ptr, bool _throw) {
  if (ptr == NULL) {
    if (_throw) {
      mac_error_throw("Memory Error: Null\n\t- Position:%s:%d\n", file, line);
    }
    return -1;
  }
  YieldToAnyThread();
  auto isRelocZone = HandleZone((Handle)ptr);

  auto readErr = GetPtrSize((Ptr)ptr);
  YieldToAnyThread();
  if (readErr < 0) {
    if (_throw) {
      switch (readErr) {
      case memROZErr:
        mac_error_throw("Memory Error: ROZ Error\n\t- Position:%s:%d\n", file,
                        line);
        break;
      case memFullErr:
        mac_error_throw("Memory Error: Not enough room in heap zone\n\t- "
                        "Position:%s:%d\n",
                        file, line);
        break;
      case nilHandleErr:
        mac_error_throw("Memory Error: Master Pointer was NIL in HandleZone "
                        "or other\n\t- "
                        "Position:%s:%d\n",
                        file, line);
        break;
      case memWZErr:
        mac_error_throw(
            "Memory Error: WhichZone failed (applied to free block)\n\t- "
            "Position:%s:%d\n",
            file, line);
        break;
      case memPurErr:
        mac_error_throw("Memory Error: trying to purge a locked or "
                        "non-purgeable block\n\t- "
                        "Position:%s:%d\n",
                        file, line);
        break;
      case memAdrErr:
        mac_error_throw("Memory Error: address was odd; or out of range\n\t- "
                        "Position:%s:%d\n",
                        file, line);
        break;
      case memAZErr:
        mac_error_throw("Memory Error: Address in zone check failed\n\t- "
                        "Position:%s:%d\n",
                        file, line);
        break;
      case memPCErr:
        mac_error_throw("Memory Error: Pointer Check failed\n\t- "
                        "Position:%s:%d\n",
                        file, line);
        break;
      case memBCErr:
        mac_error_throw("Memory Error: Block Check failed\n\t- "
                        "Position:%s:%d\n",
                        file, line);
        break;
      case memSCErr:
        mac_error_throw("Memory Error: Size Check failed\n\t- "
                        "Position:%s:%d\n",
                        file, line);
        break;
      case memLockedErr:
        mac_error_throw("Memory Error: Trying to move a locked block.\n\t- "
                        "Position:%s:%d\n",
                        file, line);
        break;
      default:
        mac_error_throw("Memory Error: Undocumented (%d).\n\t- "
                        "Position:%s:%d\n",
                        readErr, file, line);
        break;
      }
      return -1;
    } else {
      return readErr;
    }
  } else {
    return noErr;
  }

  YieldToAnyThread();
}