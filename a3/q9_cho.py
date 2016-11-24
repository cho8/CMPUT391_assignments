import sys, mmap, re


print 'Number of arguments:', len(sys.argv), 'arguments.'
print 'Argument List:', str(sys.argv)

fileSize = None
f_query = open('query.txt','w');
d_prefix = {}
d_select = {}
pattern = []

def readDataFile(filename):
    with open(filename, 'r+b') as ff:
        f = mmap.mmap(ff.fileno(), 0)
        fileSize = f.size()

        count = 0;

        # parse line
        while True:
            dataline = f.readline()
            # print(dataline);

            if re.search('PREFIX', dataline):
                parsePrefix(dataline)

            elif re.search('SELECT', dataline):
                parseSelect(dataline)

            elif re.search('WHERE', dataline):
                dataline = dataline.replace(' ', '\t').strip('\n')
                statement = dataline.split('\t')
                if statement[0].upper() == 'WHERE' and statement[1] == '{':
                    # read the next line after where
                    dataline = f.readline().strip('\t ')
                    statement = dataline.replace(' ','\t').split('\t')
                    m = re.search(':',statement[1])
                    for s in statement:
                        m = re.search(':', s)
                        if (not m==None) :
                            prefix = s[:m.start(0)+1]
                            s=s.replace(prefix, pattern[prefix])
                    pattern.append({'sub': statement[0], 'pred':statement[1], 'object':statement[2]})
                    print pattern




            elif dataline == '': #EOF
                break;

def parseSelect(dataLine):
    print("in select")
    dataLine = dataLine.replace(' ', '\t')
    statement = dataLine.split('\t')

    if statement[0].upper() == 'SELECT':
        if statement[1] == '*':
            return
        else :
            for s in statement[1:]:
                if s.upper == 'WHERE': return
                elif s== '': continue
                d_select[s.strip('\n?')] = []
                print d_select



def parsePrefix(dataLine):
    dataLine = dataLine.replace(' ', '\t');
    tag, pref, iri = dataLine.split('\t');
    # print(tag, pref, iri, term);
    if tag =='PREFIX' :
        # check the prefix
        if pref[-1] != ':':
            print(">> Missing colon")
            return False        # Error! Missing Colon

        # parse the iri
        else:
            # print("print to file")
            # f_prefix.write(pref + '\t' +iri+ '\n')
            d_prefix[pref] = iri.strip('<>')
            # print d_prefix


###### Main ###########################
if __name__ == "__main__":
    argv = sys.argv
    if len(argv) == 3:
        sqldb = argv[1]
        filename = argv[2]
        readDataFile(filename)
    else:
        print("Usage: "+ argv[0] + " <sql-database> <sparql query>\n")




##### END #############################
f_query.close()
