uint32 keusAppEvents;

void application_event_loop()
{

    if(keusAppEvents)
   {

   }
   else
   goto label;

   if(keusAppEvents & KEUS_UART)
   {
       taskChecker();
       
       keusAppEvents = keusAppEvents ^ KEUS_UART;
       goto label;
   }

   if(keusAppEvents & KEUS_UART_ACK)
   {
       fn4(x,y,z);
       
       keusAppEvents = keusAppEvents ^ KEUS_EVT4;
       return;
   }

   label:
   return;
}