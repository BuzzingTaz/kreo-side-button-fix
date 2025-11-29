#include <linux/hid.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>

/*
 * Kreo Chimera Descriptor Fixup
 * VID: 0x248a, PID: 0x5b4a
 * * The device reports "Report Count = 5" (5 bits for buttons)
 * but wrongly reports "Usage Maximum = 3" (Only 3 buttons defined).
 * We patch Usage Maximum to 5 so the kernel sees the side buttons.
 */

#define KREO_VID 0x248a
#define KREO_PID 0x5b4a

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BuzzingTaz");
MODULE_DESCRIPTION("Descriptor Fixup for Kreo Chimera");

static const __u8 *kreo_report_fixup(struct hid_device *hdev, __u8 *rdesc,
                                     unsigned int *rsize) {
  struct usb_interface *intf = to_usb_interface(hdev->dev.parent);

  // Interface 0 corresponds to the actual mouse behaviour (output from `lsusb -v`)
  if (intf->cur_altsetting->desc.bInterfaceNumber != 0)
    return rdesc;

  // 0x29, 0x03 (Usage Maximum 3) to 0x29, 0x05 (Usage Maximum 5)
  for (int i = 0; i < *rsize - 2; i++) {
    if (rdesc[i] == 0x29 && rdesc[i + 1] == 0x03) {
      rdesc[i + 1] = 0x05;
      break;
    }
  }

  return rdesc;
}

static int kreo_probe(struct hid_device *hdev, const struct hid_device_id *id) {
  int ret;

  // This triggers report_fixup internally
  ret = hid_parse(hdev);
  if (ret) {
    hid_err(hdev, "Parse failed\n");
    return ret;
  }

  // Start hardware
  ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
  if (ret) {
    hid_err(hdev, "HW start failed\n");
    return ret;
  }

  return 0;
}

static void kreo_remove(struct hid_device *hdev) { hid_hw_stop(hdev); }

static const struct hid_device_id kreo_devices[] = {
    {HID_USB_DEVICE(KREO_VID, KREO_PID)}, {}};
MODULE_DEVICE_TABLE(hid, kreo_devices);

static struct hid_driver kreo_side_driver = {
    .name = "kreo_side_fixup",
    .id_table = kreo_devices,
    .probe = kreo_probe,
    .remove = kreo_remove,
    .report_fixup = kreo_report_fixup,
};

module_hid_driver(kreo_side_driver);
