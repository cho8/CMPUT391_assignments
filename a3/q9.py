import sys, mmap, re

curr = dict()
temp = list()
counter = 0
d_prefix = {}

def parsePrefix(dataLine):
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

        if uri[-2] != '/' and uri[-2] != '#':
            print(">> Missing backslash/hashtag")
            return False


        # parse the iri
        if term.strip('\n') == '.':
            # print("print to file")
            # f_prefix.write(pref + '\t' +iri+ '\n')
            d_prefix[pref] = uri.strip('<>')
            #print (d_prefix)
        return True



def parse_rdf(file):
    flag = 'null'
    with open (file, "r", encoding = 'utf8') as a:
        with open ("parsed_results.txt", "w", encoding = 'utf8') as b:
            for lin in a:
                global counter
                counter=counter+1
                if counter == 232 or counter == 233:
                    print(lin)

                

                #recording prefix
                if "@" in lin and ('@prefix' in lin):
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

                check(curr['sub'], curr['pred'], curr['obj'])
                for i in curr:
                    if ('http' in curr[i]) or ('date' in curr[i]) or ('float' in curr[i]) or ('^' in curr[i]) or ('_:'in curr[i]):
                        continue

                    re_object = re.search(':',curr[i])
                    if re_object:
                        index = re_object.start(0)
                        print(index)

                    
                        curr[i]=curr[i].replace(curr[i][:index+1],d_prefix[curr[i][:index+1]])
                b.write(curr['sub']+"\t"+curr['pred']+"\t"+curr['obj']+"\n")
    a.close()
    b.close()

def check(sub, pred, obj):
    pass

def write_to_db(file):
    



###### Main ###########################
if __name__ == "__main__":
    argv = sys.argv
    if len(argv) == 3:
        sqldb = argv[1]
        filename = argv[2]
        parse_rdf(filename)
        write_to_db('parsed_results.txt')
    else:
        print("Usage: "+ argv[0] + " <sql-database> <rdf>\n")




##### END #############################
