file = open('raw')
output_file = open('data', 'w')

lines = file.readlines()

headers = lines[0].split(',')

types = [1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1]

lines = lines[1:]

print 'header length: ', len(headers)
print 'type length: ', len(types)

for line in lines:
    components = line.split(',')
    components[-3] = components[-3][0:4] + '-' + components[-3][4:6] + '-' + components[-3][6:]
    components[-2] = components[-2][0:4] + '-' + components[-2][4:6] + '-' + components[-2][6:]
    for i in xrange(len(components)):
        output_file.write(headers[i].strip())
        output_file.write(': ')
        if types[i] == 1:
            output_file.write('"')
            output_file.write(components[i].strip())
            output_file.write('"')
            output_file.write(' ')
        else:
            if '+' in components[i]:
                output_file.write('0')
            else:
                output_file.write(components[i].strip())
            output_file.write(' ')
    output_file.write('\n')

output_file.close()
file.close()
