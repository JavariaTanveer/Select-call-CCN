# Select-call-CCN

 char baseString[MAXLINE];

    // Get the base string from the user
    printf("Enter the base string: ");
    fgets(baseString, sizeof(baseString), stdin);
    baseString[strcspn(baseString, "\n")] = '\0'; // Remove newline character

 
 char clientIP[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(cliaddr.sin_addr), clientIP, INET_ADDRSTRLEN);
                int clientPort = ntohs(cliaddr.sin_port);
