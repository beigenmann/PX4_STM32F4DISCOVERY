/**
 * @file hc_sr04.cpp
 *
 * HC-SR04 driver.
 */
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stm32.h>
#include <nuttx/config.h>
#include <drivers/device/device.h>
#include <drivers/drv_hc_sr04.h>

#define GPIO_TRIG (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_SET|GPIO_PORTC|GPIO_PIN6)
#define GPIO_ECHO (GPIO_INPUT|GPIO_FLOAT|GPIO_EXTI|GPIO_PORTC|GPIO_PIN7)

__BEGIN_DECLS

__END_DECLS

class HC_SR04: device::CDev {
public:
	HC_SR04();
	virtual ~HC_SR04();

	virtual int init();
	virtual ssize_t read(struct file *filp, char *buffer, size_t buflen);
	virtual int ioctl(struct file *filp, int cmd, unsigned long arg);
	void print_info();
private:
	static int irq_handler(int irq, FAR void *context);
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
	stm32_configgpio(GPIO_TRIG);
	stm32_configgpio(GPIO_ECHO);
	stm32_gpiosetevent(GPIO_ECHO, false, true, false, irq_handler);

	return 0;
}
ssize_t HC_SR04::read(struct file *filp, char *buffer, size_t buflen) {
	long int a =0;
	char str1[] = "To be or not to be\n";
	struct timespec timestart;
	struct timespec timeend;
	stm32_gpiowrite(GPIO_TRIG, true);
	usleep(200);
	stm32_gpiowrite(GPIO_TRIG, false);
	usleep(1000);
	stm32_gpiowrite(GPIO_TRIG, true);
    a = a;
	clock_gettime(CLOCK_REALTIME, &timestart);

	clock_gettime(CLOCK_REALTIME, &timeend);

	a = timeend.tv_nsec - timestart.tv_nsec;

	ssize_t strl = strlen(str1);
	ssize_t ret = strl > buflen ? buflen : strl;
	memcpy(buffer, str1, ret);
	return ret;
}

int HC_SR04::ioctl(struct file *filp, int cmd, unsigned long arg) {
	int result = OK;

	switch (cmd) {

	default:
		result = CDev::ioctl(filp, cmd, arg);
	}
	return result;
}

void HC_SR04::print_info() {
}

int  HC_SR04::irq_handler(int irq, FAR void *context){

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
