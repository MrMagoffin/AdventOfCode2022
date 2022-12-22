if __name__ == "__main__":
  f = open("calories.txt")
  current_sum = 0
  sums = list()
  for line in f:
    line = line.strip()
    if len(line) == 0:
      sums.append(current_sum)
      current_sum = 0
    else:
      current_sum += int(line)
  f.close()
  if current_sum != 0:
    sums.append(current_sum)
  sums = sorted(sums)
  print(sums)
  print(sums[-1])
  print(sums[-1]+sums[-2]+sums[-3])