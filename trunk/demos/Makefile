
CFLAGS:=$(subst -I, -I../../,$(CFLAGS))


DEMOS = textdemo gameloop gamedemo linestripdemo renderbitmap

all: $(DEMOS)

clean:
	$(foreach DEMONAME,$(DEMOS), cd $(DEMONAME) && make clean && cd .. && ) cd .


.PHONY: all $(DEMOS)


export

$(DEMOS):
	cd $@ && make;


