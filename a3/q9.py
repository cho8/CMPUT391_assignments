curr = dict()
temp = list()


def parse_rdf():
   flag = 'null'
   with open ("Edmonton.txt", "r", encoding = 'utf8') as a:
      with open ("parsed_results.txt", "w", encoding = 'utf8') as b:
         for lin in a:
            if "@" in lin and ('@en' not in lin):
               #print(lin)
               continue
            # print(lin)
            # temp = lin.replace("\t", '\n').split()
            temp = lin.replace('\n','').replace('@en','').split("\t")
            # print(temp)
            # print("flag is ", flag)


            if len(temp) != 3:
               break;
 

            if (temp[0] and temp[1] and temp[2]):
               print('case 1')
               flag = temp[2][-1]
               curr['sub']=temp[0]
               curr['pred']=temp[1]
               curr['obj']=temp[2][:-2]
               check(curr['sub'], curr['pred'], curr['obj'])
               b.write(curr['sub']+"\t"+curr['pred']+"\t"+curr['obj']+"\n")
            elif (flag == ';'):
               print('case 2')
               flag = temp[2][-1]
               curr['pred'] = temp[1]
               curr['obj'] = temp[2][:-2]
               check(curr['sub'], curr['pred'], curr['obj'])
               b.write(curr['sub']+"\t"+curr['pred']+"\t"+curr['obj']+"\n")
            elif (flag == ','):
               print('case 3')
               flag = temp[2][-1]
               curr['obj'] = temp[2][:-2]
               check(curr['sub'], curr['pred'], curr['obj'])
               b.write(curr['sub']+"\t"+curr['pred']+"\t"+curr['obj']+"\n")
            else:
               print("no match", temp)








   a.close()
   b.close()

def check(sub, pred, obj):
   pass
