import cv2, cv, numpy, sys, os, math
from sets import Set


def PARSEINPUT(args, flagsSet, optionalParametersSet):

	parameters = []
	optionalParameters = {}
	flags = Set([])


	optionalParameterKey = ''
	isOptionalParameter = False

	for i in range(1,len(args)):
		arg = args[i]
		if arg[0] == '-':
			isOptionalParameter = False
			if len(arg) > 1:
				if arg[1] == '-' or len(arg) == 2:
					if arg in flagsSet:
						flags.add(arg)
					else:
						optionalParameters[arg] = []
						optionalParameterKey = arg
						isOptionalParameter = True
				else:
					for j in range(1,len(arg)):
						flags.add('-'+arg[j])

		elif isOptionalParameter:
			optionalParameters[optionalParameterKey] += [arg]
		else:
			parameters += [arg]

	return parameters, optionalParameters, flags


def merge(a,b):
	return dict(a.items() + b.items())

def prepare(directory):
	if not os.path.exists(directory):
		os.makedirs(directory)


if __name__ == '__main__':

	RULES = 'file frame [-o output] [-t time] [-f freq] [-F format]'

	SUPPORTED = Set(['avi','mp4','.mpeg','.mov','.wmv','.mkv'])

	SAMPLE_OUTPUT = '-o'
	SAMPLE_TIME = '-t'
	SAMPLE_FREQ = '-f'
	SAMPLE_FMT = '-F'

	defaultParameters = {
		SAMPLE_OUTPUT : ['o'],
		SAMPLE_TIME : [2],
		SAMPLE_FREQ : [3],
		SAMPLE_FMT : ['png']
	}

	flagsSet = Set([])
	optionalParametersSet = Set([SAMPLE_OUTPUT, SAMPLE_TIME, SAMPLE_FREQ])

	parameters, optionalParameters, flags = PARSEINPUT(sys.argv, flagsSet, optionalParametersSet)

	if len(parameters) < 2:
		print('parameters < 2')
		print(RULES)
		sys.exit(1)

	ext = str.split(parameters[0],'.')[-1]

	if ext not in SUPPORTED:
		print('input file extension not supported')
		print('must be one of ' + str(SUPPORTED))
		print(RULES)
		sys.exit(2)

	
	iFile = parameters[0]

	if os.path.isdir(iFile):
		print('file detected as a directory')
		print(RULES)
		sys.exit(3)

	if not os.path.isfile(iFile):
		print('file doesn\'t exist')
		print(RULES)
		sys.exit(4)

	try:
		iFrame = float(parameters[1])

	except ValueError:
		print('frame must be a float')
		print(RULES)
		sys.exit(5)

	optionalParameters = merge(defaultParameters, optionalParameters)

	try:
		optionalParameters[SAMPLE_TIME][0] = float(optionalParameters[SAMPLE_TIME][0])

	except ValueError:
		print('time must be a float')
		print(RULES)
		sys.exit(6)

	try:
		optionalParameters[SAMPLE_FREQ][0] = int(optionalParameters[SAMPLE_FREQ][0])
	except ValueError:
		print('freq must be a float')
		print(RULES)
		sys.exit(7)

	stream = cv2.VideoCapture(iFile)
	if not stream.isOpened():
		print('can\'t open file')
		print(RULES)
		sys.exit(8)

	fps = stream.get(cv.CV_CAP_PROP_FPS)
	fc = stream.get(cv.CV_CAP_PROP_FRAME_COUNT)

	if(fc/fps < iFrame):
		print(str(iFrame) + 's is outside the video (' + str(fc/fps) + 's).')
		print(RULES)

	else:

		stream.set(cv.CV_CAP_PROP_POS_MSEC, iFrame * 1000)

		size = int(math.ceil(fps * optionalParameters[SAMPLE_TIME][0]))
		step = int(math.ceil(fps / optionalParameters[SAMPLE_FREQ][0]))

		prepare(optionalParameters[SAMPLE_OUTPUT][0])

		for i in range(0,size):

			if(i%step == 0):

				success, image = stream.read()
				if not success:
					print('End of stream')
					break

				frameId = str(stream.get(cv.CV_CAP_PROP_POS_MSEC))
				outputFilePath = optionalParameters[SAMPLE_OUTPUT][0] + '/' + frameId + '.' + optionalParameters[SAMPLE_FMT][0]
				cv2.imwrite(outputFilePath, image)
				print('Produced image ' + outputFilePath)

			else:
				if not stream.grab():
					print('End of stream')
					break




	stream.release()

	sys.exit(0)




