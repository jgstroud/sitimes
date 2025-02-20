APP = sit95317-testapp

# Add any other object files to this list below
APP_OBJS = sit95317-testapp.o

all: build

build: $(APP)

$(APP): $(APP_OBJS)
	$(CC) -o $@ $(APP_OBJS) $(LDFLAGS) $(LDLIBS)
clean:
	rm -f $(APP) *.o

