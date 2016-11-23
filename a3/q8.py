import sys, re
import sqlite3


curr = dict() #stores the curr subj, pred, obj values
temp = list() #list that stores the parsed string
counter = 0
d_prefix = {} # dictionary that will store prefix mappings

def parsePrefix(dataLine):
    """
    checks prefix and then parses it before storing it in d_prefix
    """
    dataLine = dataLine.replace(' ', '\t');
    try:
        tag, pref, uri, term= dataLine.split('\t');
        break;
    except ValueError:
        print(">> Invalid prefix format")

    # print(tag, pref, iri, term);
    if tag =='@prefix' :
        # check the prefix
        if pref[-1] != ':':
            print(">> Missing colon")
            return False        # Error! Missing Colon
        if uri[0]!='<' and uri[-1]!='>':
            print(">> Missing opening and closing '<>'")
            return False

        if uri[-2] != '/' and uri[-2] != '#' and uri[-2] != ":":
            print(">> Missing backslash/hashtag/colon")
            return False


        # parse the iri
        if term.strip('\n') == '.':
            d_prefix[pref] = uri.strip('<>')
        else:
            print(">> Missing finalizing period")
            return False
        return True



def parse_rdf(file):
    flag = 'null'
    with open (file, "r", encoding = 'utf8') as a:
        with open ("parsed_results.txt", "w", encoding = 'utf8') as b:
            for lin in a:
                # global counter
                # counter=counter+1

                #recording prefix
                if (("@" in lin) and ('@prefix' in lin)):
                    if parsePrefix(lin) != True:
                        print("error, incorrectly formated prefix")
                        return False
                    continue

                #ignoring foreign language tags
                if "@" in lin and ('@en' not in lin):
                    continue

                #getting rid of newline and english tag identifier
                temp = lin.replace('\n','').replace('@en','').split("\t")


                if len(temp) != 3:
                   print("error because len(temp) is not 3");
                   return False


                if (temp[0] and temp[1] and temp[2]):
                    #print('case 1')
                    flag = temp[2][-1]
                    curr['sub']=temp[0]
                    curr['pred']=temp[1]
                    curr['obj']=temp[2][:-2]

                elif (flag == ';'):
                    #print('case 2')
                    flag = temp[2][-1]
                    curr['pred'] = temp[1]
                    curr['obj'] = temp[2][:-2]

                elif (flag == ','):
                    #print('case 3')
                    flag = temp[2][-1]
                    curr['obj'] = temp[2][:-2]

                else:
                    print("no match", temp)
                    return False

                check(curr['sub'], curr['pred'], curr['obj'])
                for i in curr:
                    if ('http' in curr[i]) or ('date' in curr[i]) or ('float' in curr[i]) or ('^' in curr[i]) or ('_:'in curr[i]):
                        continue

                    #replacing the prefix tag with appropriate actual URIs
                    re_object = re.search(':',curr[i])
                    if re_object:
                        index = re_object.start(0)
                        print(index)
                        curr[i]=curr[i].replace(curr[i][:index+1],d_prefix[curr[i][:index+1]])
                    if ("'s" in curr[i]):
                        curr[i] =  curr[i].replace("'s","QUOTE")

                b.write(curr['sub']+"\t"+curr['pred']+"\t"+curr['obj']+"\n")
    a.close()
    b.close()

def check(sub, pred, obj):
    if obj[0]=='"':
        if obj[-1]!='"':
            print("checking failed, unclosed quotations")
            return False



def two_same(string)
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
    print(sql_statement)
    c.execute(sql_statement)
    conn.commit()
    conn.close()


###### Main ###########################
if __name__ == "__main__":
    argv = sys.argv
    if len(argv) == 3:
        sqldb = argv[1]
        filename = argv[2]
        parse_rdf(filename)
        write_to_db(sqldb)
        #print(sqldb,filename)

    else:
        print("Usage: "+ argv[0] + " <sql-database> <rdf>\n")




##### END #############################
