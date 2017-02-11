

all : 
	@echo 0 > /sys/devices/system/cpu/cpuquiet/tegra_cpuquiet/enable
	@echo 1 > /sys/devices/system/cpu/cpu0/online
	@echo 1 > /sys/devices/system/cpu/cpu1/online
	@echo 1 > /sys/devices/system/cpu/cpu2/online
	@echo 1 > /sys/devices/system/cpu/cpu3/online
	@echo performance > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
	@echo -1 > /sys/kernel/debug/tegra_hdmi/hotplug
	@echo 4 > /sys/class/graphics/fb0/blank
	@v4l2-ctl --set-ctrl=exposure_auto=0,contrast=10,saturation=200,white_balance_temperature_auto=0,white_balance_temperature=6200,sharpness=50,backlight_compensation=0,exposure_absolute=9
	@v4l2-ctl --set-fmt-video=pixelformat=1
	g++ ${shell find . -name "*.cpp"} -o out -L. -lenet -std=c++11 -lpthread ${shell pkg-config --libs --static opencv}
	cp out /home/ubuntu/Desktop/thresh
