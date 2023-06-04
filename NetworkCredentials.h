struct NetworkCredentials 
{
  const char* Ssid;
  const char* Password;

  NetworkCredentials(const char* ssid, const char* password) : Ssid(ssid), Password(password)
  {
  }
};
