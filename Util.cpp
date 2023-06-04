//This was supposed to be a simple encoding algorithm to get a number from a string, but i got carried away and it turned into a hashing algorithm :)
//for an actual hasing algorithm i would prob just use a single loop that takes the even and odd characters (with offset) and concat those to get a more random output.
//for this algo it's important that similar inputs give similar outputs, this means that the duplicate outputs would only be generated if all unique outputs are exausted (just the compliment of strlen would be better for this) 
//well now i'm sad that i wasted time on this and didn't just do the strlen thing, but i'm not not going to use this algo when i already made it.
const char* GetSsidAndPass()
{
  const char* uidd = __DATE__;
  const char* uidt = __TIME__;

  int lengthd = strlen(uidd);
  int lengtht = strlen(uidt);
  uint64_t dateHash = 1;
  uint64_t timeHash = 1;
  for (uint8_t i = 0; i < lengthd; i++) 
  {
    uint8_t character = uidd[i];
    if (!(character % 2)) dateHash *= ((character | uidd[(dateHash)%lengthd]) * i); // even
    else dateHash ^= (i * uidd[(timeHash)%lengthd]); // odd
  }

  for (uint8_t i = 0; i < lengtht; i++) 
  {
    uint8_t character = uidt[i];
    if (!(character % 2)) timeHash ^= (i * uidd[(timeHash)%lengtht]); //even
    else timeHash *= ((character | uidd[(timeHash)%lengtht]) * i); //odd
  }

  dateHash %= (0xFFFFFFFF);
  timeHash = ~timeHash % (0xFFFFFFFF);
  uint64_t resultHash = ~(dateHash + timeHash);

  //avoid String class because otherwise everyone is going to be mad at me for using it "ThInK aBoUt ThE rAm UsAgE!!!!"... even though this is worse, but i guess i have more control here

  //First element is useless because i add 1 to the maskedbyte thingy which means it can never be 0
  const char allowedSsidCharacters[] = {
    ' ', '1', '6', 'O', '}', '7', 'S', 'y', '3', 'z', 'U', '9', 'i', 'p', 'Q', 'C', '%', '5', 'F',
    '}', 'L', 'Y', '2', 'N', '@', 'g', 'A', 'H', 'M', '0', 'j', 'e', '#', 'B', 'x', '4', '8',
    'D', 'I', 'c', 'o', 'h', 'G', '-', 'P', 'K', 's', '&', 'v', 'a', 'b', 'W',
    'r', 'f', '_', 'Z', 'n', 'q', 'R', 'X', 'k', 'd', 't', '}', 'J', 'E', 'l', 'Q',
    ':'
  };

  char outputBuffer[11];
  uint8_t i = 0;
  while (resultHash) 
  {
    uint8_t maskedByte = ((resultHash % 100) + 1) / 1.470588235294118; //get every pair of digits and map to index for 68 (+1) elements
    uint8_t index = round(maskedByte);
    outputBuffer[i++] = allowedSsidCharacters[index ? index : index + 1]; //should never be 0 but somehow it is sometimes :(
    resultHash /= 100;
  }

  outputBuffer[10] = '\0'; //GOD DAMMIT i always forget this fucker... well thats 10min of debugging wasted

  //dont need to delete static arrays, the compiler does that for me... how nice
  //delete[] allowedSsidCharacters

  return outputBuffer;
}