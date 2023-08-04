#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cmath>


 
using namespace std;
 
const int MAX_RESULT_DOCUMENT_COUNT = 5;
 
string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}
 
int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}
 
vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
 
    return words;
}
 
struct Document {
    int id;
    double relevance;
};
 
class SearchServer {
public:
    
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }
  void AddDocument(int document_id, const string& document) {
      const vector<string> words = SplitIntoWordsNoStop(document);
      document_count_++;
      const double particle= 1./words.size();
      for (string word : words){
          double word_count = 0;
          word_count+=count(words.begin(),words.end(), word); 
          word_count*=particle;
          word_to_document_freqs_[word][document_id] = word_count;
          }
      
      }
  
       
    
   
 
    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);
 
        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }
 
private:
    

    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };
     
    set<string> stop_words_;
    int document_count_ = 0;
    map<string, map<int, double>> word_to_document_freqs_;
    
 
    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }
 
    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }
    
    
 
    Query ParseQuery(const string& text) const {
        Query query;
        for (string& word : SplitIntoWords(text)) {
            if (word[0] == '-') {
                word=word.substr(1);
                if (!IsStopWord(word)) {
                    query.minus_words.insert(word);
                }
            } else  {
                query.plus_words.insert(word);
            }
        }
        return query;
    }
    
    
    
    double CalcIDF(const string& plusword) const{
        return static_cast<double> (log(static_cast<double> (document_count_) / static_cast<double> (word_to_document_freqs_.at(plusword).size())));
    }
    
    
    
    vector<Document> FindAllDocuments(const Query& query_words) const {
        map<int, double> document_to_relevance;
        vector<Document> answer;
        for (const string& plusword : query_words.plus_words) {
            if( word_to_document_freqs_.count(plusword) != 0 ){ 
                double IDF = CalcIDF(plusword);
                for( const auto& [document_id, TF] : word_to_document_freqs_.at(plusword)){
                     double TF_IDF= IDF * (TF);
                     document_to_relevance[document_id]+=TF_IDF;
                 }
            }
        }
    
       if(!query_words.minus_words.empty()){      
           for  (const auto& minusword : query_words.minus_words) {
           if( word_to_document_freqs_.count(minusword) != 0 ){
                for ( const pair<string,map<int, double>>& doc : word_to_document_freqs_){
                    if (doc.first == minusword){
                        for(const auto& [document_id, TF] : doc.second){
                           document_to_relevance.erase(document_id);
                                }
                            }
                        }
                   }
               }
           }
          
          
       for ( const auto& doc : document_to_relevance){ 
           answer.push_back({doc.first,doc.second});
                }
                  
       return answer;
            
    }
 
    
};
 
SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());
 
    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }
 
    return search_server;
}
 
int main() {
    const SearchServer search_server = CreateSearchServer();
 
    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }
    return 0;}
