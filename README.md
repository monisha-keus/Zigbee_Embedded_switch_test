# zigbee
Zigbee training code


***************************README***********************

Total 5 tasks are executed:
****Task1

1) Execute State(04) : based on saved config_id of corresponding led,input state 
		       will get validated and saved into memory.
		

Cmd:	S	len	cmd_id	*txn_id  no_of_led	<led_no  state> 	E
Ex: 	AA 0A 04 01 04 01 10 02 77 03 46 04 56 FF
	28 0B 04 01 03 01 10 02 77 03 46 04 33 29

****Task2

2) Configure Switch(0x01) : Based on the input config_id of corresponding led,saved state of Led will 
			  be validated and saved into memory.

		          config Id		    	    valid-state
			     (01) 	on/off         -  	0/255
			     (02) 	dimming        - 	0 <-> 255
			     (03) 	fan controller -  	0,50,100,150,200,255
			  

Cmd:	S	len	cmd_id	   *txn_id  no_of_led	<led_no    config_id>	E
Ex: 	AA 0A 01 04 01 02 02 02 03 02 04 02 FF
	28 09 01 01 03 01 02 02 02 03 02 29

****Task3

3) Get switch state(0x02) : Respond to Uart with led_no and it's corresponding switch state.

cmd : S	LEN	CMD_ID	txn_id	E
Ex  : 28 01 02 01 29

rply-> S   LEN	 Cmd_id	 <led_no    switch_state>	E

****Task4

4) Get config(0x03) : Respond to Uart with led_no and it's corresponding config_id.

cmd : S		LEN	CMD_ID	txn_id	     E
Ex  : 28 01 03 01 29

rply-> S   LEN	 Cmd_id	 <led_no config_id>	E 	


*****Task5

5) Get_Button_Ack(0x05) : Based on Button Pressed Event Get Ack of that particular button event

Input : Button pressed
Rply  : AA 	LEN ID 		Button_no 	Button_state 		FF


************UART 1 
