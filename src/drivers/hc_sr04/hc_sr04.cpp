/**
 * @file hc_sr04.cpp
 *
 * HC-SR04 driver.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stm32.h>
#include <semaphore.h>
#include <nuttx/config.h>
#include <drivers/device/device.h>
#include <drivers/drv_hc_sr04.h>

#define GPIO_TRIG (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_SET|GPIO_PORTC|GPIO_PIN6)
#define GPIO_ECHO (GPIO_INPUT|GPIO_FLOAT|GPIO_EXTI|GPIO_PORTC|GPIO_PIN7)
#define SELECT_LISTENER_DELAY 1000000
__BEGIN_DECLS

__END_DECLS

class HC_SR04: device::CDev {
public:
	HC_SR04();
	virtual ~HC_SR04();

	virtual int init();
	virtual ssize_t read(struct file *filp, char *buffer, size_t buflen);
	virtual int ioctl(struct file *filp, int cmd, unsigned long arg);
	void irqeEcho();
	void print_info();
private:
	timespec timeend;
	timespec abstime;
	sem_t sem_isr;
	static int irq_handler(int irq, FAR void *context);
};

HC_SR04::HC_SR04() :
		CDev("hc_sr04", HC_SR04_DEVICE_PATH), sem_isr(SEM_INITIALIZER(0)) {
	init();
	sem_init(&sem_isr, 0, 0);
	abstime.tv_sec = 0;
	abstime.tv_nsec = 1000;
}

HC_SR04::~HC_SR04() {
	sem_destroy(&sem_isr);
}

int HC_SR04::init() {
	CDev::init();
	stm32_configgpio(GPIO_TRIG);
	stm32_configgpio(GPIO_ECHO);
	stm32_gpiosetevent(GPIO_ECHO, false, true, false, irq_handler);
	return 0;
}
ssize_t HC_SR04::read(struct file *filp, char *buffer, size_t buflen) {
	char str1[] = "To be or not to be\n";
	struct timespec timestart;

	stm32_gpiowrite(GPIO_TRIG, true);
	usleep(200);
	stm32_gpiowrite(GPIO_TRIG, false);
	usleep(1000);
	stm32_gpiowrite(GPIO_TRIG, true);

	clock_gettime(CLOCK_REALTIME, &timestart);

	int ret = sem_timedwait(&sem_isr, &abstime);
	if (ret == ETIMEDOUT) {
		return snprintf(buffer, buflen, "TimeOut");
	} else {
		return snprintf(buffer, buflen, "No");
	}
	long int time_elapsed = timeend.tv_nsec - timestart.tv_nsec;
	if ((time_elapsed) > 23285) {
//			duration1 = 23285;
//			distance1 = ((duration1 / 2) / 29.1) * 10;
	} else {

	}
	ssize_t strl = strlen(str1);
	ssize_t ret2 = strl > (ssize_t) buflen ? buflen : strl;
	memcpy(buffer, str1, ret2);
	return ret2;
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

void HC_SR04::irqeEcho() {
	clock_gettime(CLOCK_REALTIME, &timeend);
}

namespace {
HC_SR04 *gHC_SR04;
}

int HC_SR04::irq_handler(int irq, FAR void *context) {

	if (gHC_SR04 != nullptr) {
		gHC_SR04->irqeEcho();
	}
}

void drv_hc_sr04_start(void) {
	if (gHC_SR04 == nullptr) {
		gHC_SR04 = new HC_SR04;
		if (gHC_SR04 != nullptr)
			gHC_SR04->init();
	}
}

int hc_sr04_main(int argc, char *argv[]) {
	timespec abstime2;
	abstime2.tv_sec = 0;
	abstime2.tv_nsec = 1000;
	sem_t sem_isr2;
	sem_init(&sem_isr2, 0, 0);
	int ret = sem_timedwait(&sem_isr2, &abstime2);
	if (ret != OK) {
		printf("!ok\n", ret);
	} else {
		printf("ok\n");
	}

//	drv_hc_sr04_start();
}
