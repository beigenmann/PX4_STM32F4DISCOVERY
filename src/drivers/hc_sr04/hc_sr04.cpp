
/**
 * @file hc_sr04.cpp
 *
 * HC-SR04 driver.
 */

#include <nuttx/config.h>
#include <drivers/device/device.h>
#include <drivers/drv_hc_sr04.h>


__BEGIN_DECLS

__END_DECLS

class HC_SR04: device::CDev {
public:
	HC_SR04();
	virtual ~HC_SR04();

	virtual int init();
	virtual int ioctl(struct file *filp, int cmd, unsigned long arg);
};

HC_SR04::HC_SR04() :
		CDev("hc_sr04", HC_SR04_DEVICE_PATH) {
	// force immediate init/device registration
	init();
}

HC_SR04::~HC_SR04() {
}

int HC_SR04::init() {
	CDev::init();
	return 0;
}

int HC_SR04::ioctl(struct file *filp, int cmd, unsigned long arg) {
	int result = OK;

	switch (cmd) {

	default:
		result = CDev::ioctl(filp, cmd, arg);
	}
	return result;
}

namespace {
HC_SR04 *gHC_SR04;
}

void drv_hc_sr04_start(void) {
	if (gHC_SR04 == nullptr) {
		gHC_SR04 = new HC_SR04;
		if (gHC_SR04 != nullptr)
			gHC_SR04->init();
	}
}
