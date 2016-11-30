from collections import Counter
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
    tag, pref, uri, term= dataLine.split('\t');
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
    line_number = 0
    flag = 'null'

    with open (file, "r", encoding = 'utf8') as a:
        with open ("parsed_results.txt", "w", encoding = 'utf8') as b:
            for lin in a:
                print(flag,line_number)
                line_number = line_number+1
                # global counter
                # counter=counter+1

                #recording prefix
                if (("@" in lin) and ('@prefix' in lin)):
                    if parsePrefix(lin) != True:
                        print("error, incorrectly formated prefix",line_number)
                        return False
                    continue

                #ignoring foreign language tags
                if "@" in lin and ('@en' not in lin):
                    temp = lin.replace('\n','').split("\t")
                    flag = temp[2][-1]
                    if '@' not in temp[0]:
                        curr['sub']=temp[0]
                    if '@' not in temp[1]:
                        curr['pred']=temp[1]
                    if '@' not in temp[2]:
                        curr['obj']=temp[2][:-2]
                    continue

                    


                #getting rid of newline and english tag identifier
                temp = lin.replace('\n','').replace('@en','').split("\t")


                if len(temp) != 3:
                   print("error because len(temp) is not 3",line_number);
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
                    print("no match", temp,line_number,flag)
                    return False

                if not check(curr['sub'], curr['pred'], curr['obj']):
                    return False
                for i in curr:
                    if ('http' in curr[i]) or ('date' in curr[i]) or ('float' in curr[i]) or ('^' in curr[i]) or ('_:'in curr[i]):
                        continue

                    #replacing the prefix tag with appropriate actual URIs
                    re_object = re.search(':',curr[i])
                    if re_object:
                        index = re_object.start(0)
                        #print(index)
                        curr[i]=curr[i].replace(curr[i][:index+1],d_prefix[curr[i][:index+1]])
                    if ("'s" in curr[i]):
                        curr[i] =  curr[i].replace("'s","QUOTEs")

                b.write(curr['sub']+"\t"+curr['pred']+"\t"+curr['obj']+"\n")
    a.close()
    b.close()
    return True

def check(sub, pred, obj):

    #check for valid string
    if '"' in obj:
        if (obj.count('"')!=2):
            print("Wrongly formatted string", obj)
            return False

    #check for valid date
    if 'xsd:date' in obj:
        if ((obj[0] != '"') and (obj[-11] != '"')):
            print("wrongly formatd date")
            return False
        if  ((obj[1:5].isdigit() == False) or (obj[6:8].isdigit() == False) or (obj[9:11].isdigit() == False)):
            print("wrongly formatted date")
            return False
        if ((obj[5] !="-") or (obj[8] != "-")):
            print("wrongly formatted date")
            return False

    #check for valid float
    if 'float' in obj:
        value = obj[1:-12]
        for i in value:
            if i.isdigit() == False and i != '.' and i !='-':
                print("error validating floats", i, value)
                return False
        if obj.count('.') != 1:
            return False
        if obj.count('-') > 1:
            return False

    #check for non Negative integers
    if 'nonNegativeInteger' in obj:
        if obj[1:-25].isdigit() == False:
            print("wrongly formatted nonNegativeInteger")
            return False

    if 'Geometry' in obj:
        value = obj[7:-20]
        for i in value:
            if ((i != '-') and (i.isdigit()==False) and (i != ' ') and (i != '.')):
                print("error processing Geometry")
                return False
        if '.' in value:
            if value.count('.')>2:
                print("error expected only 2 periods")
                return False
        if value.count(" ") !=1:
            print("error expected 1 space")
            return False

    return True






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
    #c.execute ('CREATE TABLE rdf (sub TEXT, pred TEXT, obj TEXT)')

    data = ''

    with open('parsed_results.txt', 'r',encoding = 'utf8') as rslt:
        for lin in rslt:
            result = lin.split('\t')
            triple = '(\''+result[0]+'\',\''+result[1]+'\',\''+result[2]+'\')'
            data = data+triple+','
    sql_statement = "INSERT INTO rdf VALUES" + data[0:-1] + ";"
    #print(sql_statement)
    c.execute(sql_statement)
    conn.commit()
    conn.close()


###### Main ###########################
if __name__ == "__main__":
    argv = sys.argv
    if len(argv) == 3:
        sqldb = argv[1]
        filename = argv[2]
        if (parse_rdf(filename)):
            print("processing passed, writing to db")
            write_to_db(sqldb)
        else:
            print("error processing the document")
    else:
        print("Usage: "+ argv[0] + " <sql-database> <rdf>\n")




##### END #############################
