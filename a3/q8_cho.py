import sys, mmap, re


print 'Number of arguments:', len(sys.argv), 'arguments.'
print 'Argument List:', str(sys.argv)

fileSize = None
f_prefix = open('prefix.txt','w');

def readDataFile(filename):
    with open(filename, 'r+b') as ff:
        f = mmap.mmap(ff.fileno(), 0)
        fileSize = f.size()

        count = 0;

        # parse line
        while True:
            dataLine = f.readline()
            lineArr = dataLine.split("\t")

            if re.search('@prefix', lineArr[0]):
                parsePrefix(dataLine)
            elif dataLine == '': #EOF
                break;



def parsePrefix(dataLine):
    dataLine = dataLine.replace(' ', '\t');
    tag, pref, iri, term= dataLine.split('\t');
    print(tag, pref, iri, term);
    if tag =='@prefix' :
        # check the prefix
        if pref[-1] != ':':
            print(">> Missing colon")
            return False        # Error! Missing Colon

        # parse the iri
        if term.strip('\n') == '.':
            print("print to file")
            f_prefix.write(pref + '\t' +iri+ '\n')


###### Main ###########################
if __name__ == "__main__":
    argv = sys.argv
    if len(argv) == 3:
        sqldb = argv[1]
        filename = argv[2]
        readDataFile(filename)
    else:
        print("Usage: "+ argv[0] + " <sql-database> <rdf>\n")




##### END #############################
f_prefix.close()
