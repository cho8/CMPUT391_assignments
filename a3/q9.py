import sys, re
import sqlite3


curr = dict() #stores the curr subj, pred, obj values
temp = list() #list that stores the parsed string
counter = 0
d_prefix = {} # dictionary that will store prefix mappings
select = {} # variables
pattern_list = [] # collection of patterns to retrieve
full_result = None # set of tuples of sparql query result

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
                select[s.strip('\n')]=set();
                print('SELECT: ',select)
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
                            if (statement==''):
                                continue

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
                            if num_var == 1:
                                pattern_list.append({'sub': statement[0], 'pred':statement[1], 'obj':statement[2]})
                            else:
                                pattern_list.insert(0,{'sub': statement[0], 'pred':statement[1], 'obj':statement[2]})
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
    basequery = 'SELECT * FROM rdf'
    # TODO append query with where clauses
    while pattern_list:
        print('############################')
        pattern = pattern_list.pop()
        print('PATTERN: ', pattern)
        var = [i for i in pattern.items() if '?' in i[1]]
        # print("stuff", var)

        # Only one variable
        if len(var)==1:
            print("SINGLE VAR --")
            # Case 1: var is sub
            if var[0][0]=='sub':
                query = basequery.replace('*', var[0][0])
                query = query +" where pred=? and obj=?;"
                c.execute(query, (pattern['pred'], pattern['obj']))
                output = c.fetchall()
                print('QUERY: ',query)
                print('OUTPUT: ',output)

                # if we don't have this subject, save it
                if (select[var[0][1]]):
                    select[var[0][1]]=select[var[0][1]].intersect(set([i[0] for i in output]))
                    pass
                else:
                    select[var[0][1]]=set([i[0] for i in output])
                    print(select)
                    pass

            # Case 2: var is obj
            elif var[0][0]=='obj':
                # query = basequery.replace('*', var[0][0])
                query = basequery +" where pred=? and sub=?;"
                c.execute(query, (pattern['pred'], pattern['sub']))


                # if we don't have this object, save it
                if (select[var[0][1]]):
                    # select[var[0][1]]=select[var[0][1]].intersect(set(c.execute(query)))
                    pass
                else:
                    # select[var[0][1]]=set(c.execute(query))
                    pass

            # Case 3: var is pred
            elif var[0][0]=='pred':
                 query = basequery +" where pred=? and sub=?;"
                 c.execute(query, (pattern['pred'], pattern['sub']))

            # print(select)
            # print('QUERY :', query)


        # Two var in one pattern
        elif len(var) == 2:
            print("DOUBLE VAR --")
            # var1 is subject, var2 is object
            # separate out spo
            sub = [i for i in pattern.items() if i[0]=='sub'][0]    # ( 'sub', '?var')
            obj = [i for i in pattern.items() if i[0]=='obj'][0]
            pred = [i for i in pattern.items() if i[0]=='pred'][0]
            # select * from rdf where
            print('SPO :', sub, pred, obj)

            if not sub or not obj:
                print ('>> Pattern parse failure')
                return False;

            # Get all the sub, obj pairs, assuming var in sub and obj
            c.execute('select sub, obj from rdf where pred=?', (pred[1],))
            output = c.fetchall()
            print('OUTPUT: ',output)

            # Relate selected variables (filter out all the stuff we didn't select before)
            # build result tuples

            # Case 1, obj list is empty, get all results with sub constraint
            if not select[obj[1]] and select[sub[1]]:
                output = [o for o in output if o[0] in select[sub[1]]]
            # Case 2, sub list is empty, get all results with obj constraint
            elif not select[sub[0][1]] and select[obj[0][1]]:
                output = [o for o in output if o[0] in select[obj[1]]]
            # Case 3, sub list and obj list constrain
            elif select[sub[0][1]] and select [obj[0][1]]:
                output = [o for o in output if o[0] in select[sub[1]] and o[1] in select[obj[1]]]

            global full_result;
            if full_result:
                full_result.intersect(set(output))
            else:
                full_result = set(output)

        # END two var pattern
    conn.commit()
    conn.close()

def print_result():
    res = list(full_result)
    for r in res:
        print(r)


###### Main ###########################
if __name__ == "__main__":
    argv = sys.argv
    if len(argv) == 3:
        sqldb = argv[1]
        filename = argv[2]
        parse_sparql(filename)
        read_from_db(sqldb)
        print_result();
        #print(sqldb,filename)

    else:
        print("Usage: "+ argv[0] + " <sql-database> <sparql-query>\n")




##### END #############################
