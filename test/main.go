package main

import (
	//"encoding/hex"
	"fmt"
	"math"
	//"github.com/harmony-one/vdf/src/vdf_go"
)

func main() {
	input := [32]byte{0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe,
		0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef}
	vdf := vdf_go.New(int(math.Pow(2, 10)), input)

	outputChannel := vdf.GetOutputChannel()
	//start := time.Now()

	vdf.Execute()

	//duration := time.Now().Sub(start)

	//fmt.Println("VDF computation finished, time spent %s", duration.String())

	output := <-outputChannel

	//fmt.Println("VDF computation finished, result is  %s", hex.EncodeToString(output[:]))

	//start = time.Now()

	result := vdf.Verify(output)

	//duration = time.Now().Sub(start)

	//fmt.Println("VDF verification finished, time spent %s", duration.String())

	if result {
		fmt.Println("True Proof")
	} else {
		fmt.Println("False")
	}

}
