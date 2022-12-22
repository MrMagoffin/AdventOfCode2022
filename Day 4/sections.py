if __name__ == "__main__":
  f = open("sections.txt")
  out_f = open("sections_out.txt", "w")
  enclose_count = 0
  overlap_count = 0
  for line in f:
    line = line.strip()
    line_split = line.split(",")
    elves = [line_split[0].split("-"), line_split[1].split("-")]
    elves[0][0] = int(elves[0][0])
    elves[0][1] = int(elves[0][1])
    elves[1][0] = int(elves[1][0])
    elves[1][1] = int(elves[1][1])
    # if the source lines is a-b,c-d, elves is now [[a, b], [c, d]]
    if elves[0][0] == elves[1][0]:
      out_f.write(f"{elves[0]} and {elves[1]} share a starting endpoint\n")
      enclose_count += 1
      overlap_count += 1
    else:
      # reorganize the two to ensure a < c
      if elves[0][0] > elves[1][0]:
        elves.append(elves[0])
        elves = elves[1:3]
      # now if b >= d, the first encloses the second
      if elves[0][1] >= elves[1][1]:
        out_f.write(f"{elves[0]} encloses {elves[1]}\n")
        enclose_count += 1
        overlap_count += 1
      else:
        if elves[0][1] < elves[1][0]:
          out_f.write(f"{elves[0]} does not overlap {elves[1]}\n")
        else:
          out_f.write(f"{elves[0]} does not enclose {elves[1]}\n")
          overlap_count += 1
  out_f.write(f"{enclose_count} have an enclosure\n")
  out_f.write(f"{overlap_count} have an overlap\n")
  f.close()
  out_f.close()