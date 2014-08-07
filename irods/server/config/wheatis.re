##############################
######### BEGIN TGAC #########
##############################
acPostProcForPut 
{ 
	writeLine ("serverLog","wheatis.re :: USER, OBJPATH, and FILEPATH: $userNameClient, $objPath and $filePath");
		
	*email_result = msiSendMail("simon.tyrrell@tgac.ac.uk","irods wheatis","User $userNameClient just ingested file $objPath into $rescName.");

	
	if (*email_result == 0)
		{
			writeLine ("serverLog","sent email succesfully");		
		}
	else
		{
			writeLine ("serverLog","send email failed - code *result");	
		}

	
	# pass the filename to each of our microservices to see if they are interested in it
	
	
}

