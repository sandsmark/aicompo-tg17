package main

import (
	"bufio"
	"math/rand"
	"net"
	"os"
)

func main() {

	// Thanks gofmt
	moves := map[int]string{
		0: "ACCELERATE",
		1: "MINE",
		2: "RIGHT",
		3: "LEFT",
		4: "MISSILE",
		5: "SEEKING"}

	conn, _ := net.Dial("tcp", "127.0.0.1:54321")
	buf := bufio.NewReader(conn)
	for {
		_, _, err := buf.ReadLine()
		if err != nil {
			os.Exit(0)
		}
		move := rand.Intn(5)

		get_move := moves[move]

		conn.Write([]byte(get_move + "\n"))
	}
}
