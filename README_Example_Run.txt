// AFTER ANY OF COMMAND, DEBUGGING PRINTS CAN BE SEEN VIA dmesg COMMAND

						Run In Order:

./install_script.sh
echo oneringtorulethemall > /dev/vault		(input is dividable with permutation key)
tac /dev/vault					(get decrypted output)

echo oneringtorulethemal > /dev/vault		(one l is missing for 0 fill testing)
tac /dev/vault					(get decrypted output)


// CHANGE PERMUTATION KEY WITH IOCTL
./app_change_key				//run test program
ceayf					//enter permutation key

//REPEAT WITH NEW PERMUTATION KEY
echo oneringtorulethemall > /dev/vault		(input is dividable with permutation key)
tac /dev/vault					(get decrypted output)

echo oneringtorulethemal > /dev/vault		(one l is missing for 0 fill testing)
tac /dev/vault	





//TESTING READ-WRITE SYNCHRONIZATION
// (with initial permutation_key = "abcd")
./remove_script.sh			(remove and reinstall from beginning)
./install_script.sh

echo aaaa > /dev/vault
echo bbbb > /dev/vault
	output: write error
tac /dev/vault
	output: aaaa
echo bbbb > /dev/vault
tac /dev/vault
	output: bbbb


//TESTING CLEARING CONTENT

./remove_script.sh			(remove and reinstall from beginning)
./install_script.sh
echo aaaa > /dev/vault
./app_clear_content
tac /dev/vault
	output: read error		(because content was cleared)













