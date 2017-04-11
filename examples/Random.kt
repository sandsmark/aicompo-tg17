package BotsRandom

import java.net.Socket
import java.util.Random;

fun main(args : Array<String>) {

    val socket = Socket("localhost", 54321)
    val ow = socket.getOutputStream()
    ow.write(("NAME rand0m\n").toByteArray())

    while (true) {
        try {

            val rn = Random()
            val i = rn.nextInt() % 4

            when (i) {
                0 -> ow.write(("up\n").toByteArray())
                1 -> ow.write(("down\n").toByteArray())
                2 -> ow.write(("left\n").toByteArray())
                3 -> ow.write(("right\n").toByteArray())
            }

            Thread.sleep(1)

        } catch (e: Exception) {
            println(e.toString())
        }
    }

    socket.close()
}