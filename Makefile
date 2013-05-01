default: demo

demo: ./tm/demo.c ./tm/tmCmd.c ./tm/tm.c
	cc -o demo ./tm/demo.c ./tm/tmCmd.c ./tm/tm.c
