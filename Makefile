

all : 
	./setup.sh
	v4l2-ctl --set-ctrl=exposure_auto=0,contrast=10,saturation=200,white_balance_temperature_auto=0,white_balance_temperature=6200,sharpness=50,backlight_compensation=0,exposure_absolute=9
	v4l2-ctl --set-fmt-video=pixelformat=1
	g++ ${shell find . -name "*.cpp"} -o out -L. -lenet -std=c++11 -lpthread ${shell pkg-config --libs --static opencv}
	cp out /home/ubuntu/Desktop/thresh
	
