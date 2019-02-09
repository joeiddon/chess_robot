import os, datetime

#nice command to print outcomes:
# for f in $(ls -1tr); do echo $f; tail -n 23 $f; printf '\n\n'; done

time_format = '%d-%m-%y@%H-%M-%S'
max_game_time = 600 #seconds
num_games_to_play = 10

for n in range(num_games_to_play):
    print(f'playing game: {n+1}/10')
    #cba to setup subprocess pipes, so using depreciated os.system()
    os.system(f'timeout {max_game_time}s ../chess_ai > ai_vs_ai/{datetime.datetime.now().strftime(time_format)}')
