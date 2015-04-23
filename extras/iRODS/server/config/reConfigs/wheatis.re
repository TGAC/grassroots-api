##############################
######### BEGIN TGAC #########
##############################
acPostProcForPut 
{ 
	writeLine ("serverLog","wheatis.re :: USER, OBJPATH, and FILEPATH: $userNameClient, $objPath and $filePath");
	
	writeLine ("serverLog","wheatis.re :: 1");
	
	*email_result = msiSendMail("simon.tyrrell@tgac.ac.uk","irods wheatis","User $userNameClient just ingested file $objPath into $rescName.");

	writeLine ("serverLog","wheatis.re ::2");

	
	if (*email_result == 0)
		{
			writeLine ("serverLog","sent email succesfully");		
		}
	else
		{
			writeLine ("serverLog","send email failed - code *result");	
		}

	
}

