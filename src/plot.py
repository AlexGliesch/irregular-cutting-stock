import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import scipy.spatial, random, itertools, sys

with open(sys.argv[1]) as f:
	d = f.readlines()

	plateSize = int(d[0])
	i = 1
	plate = []
	for j in range(plateSize):
		# print d[i].split(' ')
		plate.append(map(float, d[i].split(' ')))
		i = i + 1
	# print plate 

	pieces = []
	nPieces = int(d[i])
	i = i + 1
	for j in range(nPieces):
		pieceSize = int(d[i])
		i = i + 1
		piece = []
		for k in range(pieceSize):			
			piece.append(map(float, d[i].split(' ')))
			i = i + 1
		pieces.append(piece)
	# print '\n'.join(map(str, pieces))

plt.axis('off')
plt.fill(*zip(*plate), alpha=1.0, facecolor='lightgrey', edgecolor='black')
for piece in pieces:
	plt.fill(*zip(*piece), alpha=1.0, facecolor='lightblue', edgecolor='black',linewidth=1)

# plt.margins(0, 0)
plt.gca().axes.get_xaxis().set_ticks([])
plt.gca().axes.get_yaxis().set_ticks([])
# plt.subplots_adjust(top = 1, bottom = 0, right = 1, left = 0, 
            # hspace = 0, wspace = 0)
plt.savefig(sys.argv[1].replace('.txt', '') + '.pdf', 
	bbox_inches = 'tight', pad_inches=0.01)

