

%.o: %.s
	mips-linux-gnu-as $< -o $@

%: %.o
	mips-linux-gnu-ld $< -o $@
