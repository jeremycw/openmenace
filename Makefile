OBJS=main.o readfile.o map.o gfx.o gfx_decoder.o
CFLAGS += -Wall -Wextra -Werror -g

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

biomenace: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJS) biomenace
