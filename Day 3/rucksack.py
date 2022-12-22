# Return priority for a given character
def priority(char):
  if char.isupper():
    # Uppercase characters have ASCII values 65-90 and priorities 27-52, a difference of 38
    return ord(char) - 38
  else:
    # Lowercase characters have ASCII values 97-122 and priorities 1-26, a difference of 96
    return ord(char) - 96

if __name__ == "__main__":
  f = open("rucksacks.txt")
  priority_sum = 0 # Sum of priorities for shared values between first and second halves of a line
  set_priority_sum = 0 # Sum of priorities for shared values between consecutive three lines
  set_count = 0 # Tracking which of the three lines within a set of three we're on
  for line in f:
    line = line.strip() # Remove whitespace from the end of the line
    
    if set_count == 0: # First line of a set of three
      comp_string = line # Just set the comparison string for the set to the first line, could be any of them
      set_count = set_count + 1
    else:
      # Second of third line of set of three lines
      # Create a new comparison string with the characters in the current comparison string that are in the current line
      temp = ""
      for c in comp_string:
        if c in line:
          temp += c
      comp_string = temp
      set_count = set_count + 1
    if set_count == 3:
      # At the end of a set of three lines
      set_count = 0
      comp_string = comp_string[0] # Might have multiple instances of the same shared character, reduce to one
      print(f"Past 3 lines share character {comp_string}, priority {priority(comp_string)}")
      set_priority_sum += priority(comp_string)
      print(f"Current priority sum is {set_priority_sum}")
      
    # Split line into sacks  
    sack1 = line[0:len(line)//2]
    sack2 = line[len(line)//2:len(line)]
    # Iterate through first sack and search for that item in the second half
    found = None
    for c in sack1:
      if c in sack2:
        found = c
        break
    if found == None:
      print(f"No error found in sacks {sack1} and {sack2}")
    else:
      print(f"{found} is shared between {sack1} and {sack2}")
      priority_sum += priority(found)
      print(f"Current sum is {priority_sum}")
  f.close()