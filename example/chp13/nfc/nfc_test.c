// This code example is directly from www.libnfc.org

#include <stdlib.h>
#include <nfc/nfc.h>
char secretCode[] = {0x8e, 0x3f, 0x34, 0x03};

static void print_hex(const uint8_t *pbtData, const size_t szBytes) {
  size_t  szPos;
  for (szPos = 0; szPos < szBytes; szPos++) {
    printf("%02x  ", pbtData[szPos]);
  }
  printf("\n");
}

static bool isSecretCode(const uint8_t *pbtData, const size_t szBytes) {
  size_t  szPos;
  for (szPos = 0; szPos < szBytes; szPos++) {
    if(szPos > (sizeof(secretCode)/sizeof(char))) break;
    if(pbtData[szPos]!=secretCode[szPos]) return false;
  }
  return true;
}

int main(int argc, const char *argv[]) {
  nfc_device *pnd;
  nfc_target nt;
  // Allocate only a pointer to nfc_context
  nfc_context *context;
  // Initialize libnfc and set the nfc_context
  nfc_init(&context);
  if (context == NULL) {
    printf("Unable to init libnfc (malloc)\n");
    exit(EXIT_FAILURE);
  }
  // Display libnfc version
  const char *acLibnfcVersion = nfc_version();
  (void)argc;
  //printf("%s uses libnfc %s\n", argv[0], acLibnfcVersion);

  // Open, using the first available NFC device which can be in order of selection:
  //   - default device specified using environment variable or
  //   - first specified device in libnfc.conf (/etc/nfc) or
  //   - first specified device in device-configuration directory (/etc/nfc/devices.d) or
  //   - first auto-detected (if feature is not disabled in libnfc.conf) device
  pnd = nfc_open(context, NULL);
  if (pnd == NULL) {
    printf("ERROR: %s\n", "Unable to open NFC device.");
    exit(EXIT_FAILURE);
  }
  // Set opened NFC device to initiator mode
  if (nfc_initiator_init(pnd) < 0) {
    nfc_perror(pnd, "nfc_initiator_init");
    exit(EXIT_FAILURE);
  }
  printf("ERPi NFC reader: %s opened\n", nfc_device_get_name(pnd));
  printf(" Waiting for you to use an RFID card or tag....\n");
  // Poll for a ISO14443A (MIFARE) tag
  const nfc_modulation nmMifare = {
    .nmt = NMT_ISO14443A,
    .nbr = NBR_106,
  };
  if (nfc_initiator_select_passive_target(pnd, nmMifare, NULL, 0, &nt) > 0) {
    printf("The following tag was found:\n");
    printf("  UID (NFCID%c): ", (nt.nti.nai.abtUid[0] == 0x08 ? '3' : '1'));
    print_hex(nt.nti.nai.abtUid, nt.nti.nai.szUidLen);
    if(isSecretCode(nt.nti.nai.abtUid, nt.nti.nai.szUidLen)) {
      printf(" *** ERPi Access allowed! ***\n");
    }
    else {
      printf(" *** ERPi Access NOT allowed! ***\n");
    }
  }
  // Close NFC device
  nfc_close(pnd);
  // Release the context
  nfc_exit(context);
  exit(EXIT_SUCCESS);
}
