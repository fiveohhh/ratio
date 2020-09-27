selector_tester: main.c gear_selector.c
	gcc -o selector_tester main.c gear_selector.c

clean:
	rm -f selector_tester

test: selector_tester
	./selector_tester