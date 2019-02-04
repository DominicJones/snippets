def cypher(message, key = 13):
  result = ""
  a_value = ord('a')

  for letter in message:

    value = ord(letter)
    value_0 = value - a_value

    if letter.islower():
      value_0 = (value_0 + key) % 26

    value = value_0 + a_value
    result = result + chr(value)

  return result


print(cypher("nyrn vnpgn rfg"))
