outcome_score = {"A": {"X": 3, "Y": 6, "Z": 0}, "B": {"X": 0, "Y": 3, "Z": 6}, "C": {"X": 6, "Y": 0, "Z": 3}}
strategy_lookup = {"A": {"X": "Z", "Y": "X", "Z": "Y"}, "B": {"X": "X", "Y": "Y", "Z": "Z"}, "C": {"X": "Y", "Y": "Z", "Z": "X"}}
choice_score = {"X": 1, "Y": 2, "Z": 3}

if __name__ == "__main__":
  f = open("rps.txt")
  score = 0
  new_strat_score = 0
  for line in f:
    if len(line) >= 3:
      score += outcome_score[line[0]][line[2]] + choice_score[line[2]]
      new_strat_choice = strategy_lookup[line[0]][line[2]]
      new_strat_score += outcome_score[line[0]][new_strat_choice] + choice_score[new_strat_choice]
  f.close()
  print(score)
  print(new_strat_score)