import sys, re
import sqlite3


curr = dict() #stores the curr subj, pred, obj values
temp = list() #list that stores the parsed string
counter = 0
d_prefix = {} # dictionary that will store prefix mappings
select = [] # variables
pattern_list = [] # collection of patterns to retrieve

def parsePrefix(dataLine):
    """
    checks prefix and then parses it before storing it in d_prefix
    """
    dataLine = dataLine.replace(' ', '\t').strip('\n');
    lin = dataLine.split('\t');

    # print(tag, pref, iri, term);
    while(lin):
        tag = lin.pop(0)
        # Find where PREFIX is indicated
        if tag =='PREFIX' :

            # check the prefix
            seg = lin.pop(0)
            if seg[-1]==":" :       # case 1: PREFIX pref: <uri> (space after pref)
                pref = seg
                uri = lin.pop(0)

                if uri[0]!='<' and uri[-1]!='>':
                    print(">> Missing opening and closing '<>'")
                    return False

                if ' ' in uri:
                    print(">> Malformed URI")
                    return False

                if uri[-2] != '/' and uri[-2] != '#' and uri[-2] != ":":
                    print(">> Missing backslash/hashtag/colon")
                    return False

            elif re.search("<[\w.:/#]+>", seg) :     # case 2: PREFIX pref:<uri> (no space)
                m = re.search("<[\w.:/#]+>", seg)
                pref = seg[:m.start()-1]
                uri = seg[m.start()+1:m.end()-1]

            else:
                print(">> Malformed prefix declaration")
                return False

        # parse the iri
        if pref and uri:
            d_prefix[pref] = uri.strip('<>')
            # print(d_prefix)
        else:
            print(">> Malformed prefix declaration")
            return False
        return True

def parseSelect(dataLine):
    """
    Parses and stores the variables used in select
    """
    dataLine = dataLine.replace(' ', '\t')
    statement = dataLine.split('\t')

    if statement[0].upper() == 'SELECT':
        if statement[1] == '*':
            print("### TODO: All variables Return");
            return True
        else :
            for s in statement[1:]:
                if s.upper == 'WHERE':
                    return
                elif s== '':
                    continue
                select.append(s.strip('\n'))
                # print(select)
            return True


def parse_sparql(file):
    flag = 'null'
    with open (file, "r", encoding = 'utf8') as a:
        # with open ("parsed_results.txt", "w", encoding = 'utf8') as b:
            for lin in a:
                # global counter
                # counter=counter+1

                #recording prefix
                if ("PREFIX" in lin):
                    if parsePrefix(lin) != True:
                        print("error, incorrectly formatted prefix")
                        return False
                    continue

                elif ("SELECT" in lin):
                    if parseSelect(lin) != True:
                        print("error, incorrectly formatted select statement")
                        return False
                    continue

                # Parse WHERE patterns
                elif ("WHERE" in lin):
                    dataline = lin.replace(' ', '\t').strip('\n')
                    statement = dataline.split('\t')
                    if statement[0].upper() == 'WHERE' and statement[1] == '{':
                        # read the next line after where
                        dataline = a.readline().strip('\t\n ')
                        statement = dataline.replace(' ','\t').split('\t')

                        while (statement[0] != '}'):    # while within WHERE block
                            # map prefixes
                            i = 0;
                            for s in statement:
                                m = re.search(':', s)

                                # TODO Filter

                                # If a prefix is used
                                if m :
                                    prefix = s[:m.start(0)+1]

                                    if not prefix:
                                        print(">> Undefined prefix '{}'".format(prefix))
                                        return false

                                    s=s.replace(prefix, d_prefix[prefix])
                                    statement[i]=s
                                i+=1

                            # find if more than one variable in pattern_stack, rearrange stack
                            num_var = sum(1 for s in statement if '?' in s)
                            if num_var <= 1:
                                pattern_list.append({'sub': statement[0], 'pred':statement[1], 'object':statement[2]})
                            else:
                                pattern_list.insert(0,{'sub': statement[0], 'pred':statement[1], 'object':statement[2]})
                            # print(pattern_list)

                            # read the next line
                            dataline = a.readline().strip('\t\n ')
                            statement = dataline.replace(' ','\t').split('\t')

                        # end while
                    # end elif WHERE
                # EOF, get the data from DB.
                # end function

    a.close()
    # b.close()

def check(sub, pred, obj):
    if obj[0]=='"':
        if obj[-1]!='"':
            print("checking failed, unclosed quotations")
            return False



def two_same(string):
   for i in range(len(string)-1):
      if string[i] == string[i+1]:
         return True
   return False

def write_to_db(sqldb):
    """
    Writes to DB
    """
    conn = sqlite3.connect(sqldb)
    c = conn.cursor()
    c.execute ('CREATE TABLE rdf (sub TEXT, pred TEXT, obj TEXT)')

    data = ''

    with open('parsed_results.txt', 'r') as rslt:
        for lin in rslt:
            result = lin.split('\t')
            triple = '(\''+result[0]+'\',\''+result[1]+'\',\''+result[2]+'\')'
            data = data+triple+','
    sql_statement = "INSERT INTO rdf VALUES" + data[0:-1] + ";"
    # print(sql_statement)
    c.execute(sql_statement)
    conn.commit()
    conn.close()

def read_from_db(sqldb):
    """
    Writes to DB
    """
    conn = sqlite3.connect(sqldb)
    c = conn.cursor()
    query = 'SELECT * FROM rdf'
    # TODO appen query with where clauses

    for row in c.execute(query):
        print(row)

    conn.commit()
    conn.close()


###### Main ###########################
if __name__ == "__main__":
    argv = sys.argv
    if len(argv) == 3:
        sqldb = argv[1]
        filename = argv[2]
        parse_sparql(filename)
        read_from_db(sqldb)
        #print(sqldb,filename)

    else:
        print("Usage: "+ argv[0] + " <sql-database> <sparql-query>\n")




##### END #############################
