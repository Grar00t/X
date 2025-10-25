#include "AiReasoner.h"

QString AiReasoner::answer(const QString& user){
    const QString clean = user.left(4000);
    if(clean.contains("rm -rf") || clean.contains("format C:"))
        return "لن أنفّذ أو أشرح أوامر مدمّرة. اطلب هدفًا آمنًا.";

    if(llmEnabled()){
        const QString sys = "أجب بدقة بالعربية أو الإنجليزية حسب السؤال. إن غابت المعرفة اليقينية قل: (لا أستطيع الإجابة بثقة). "
                            "التزم بالحقائق وروح الاختصار. درجة حرارة=0.";
        auto d1 = llama_.infer(sys, clean, 0.0, 0.9);
        auto d2 = llama_.infer(sys, clean, 0.0, 0.9);
        const int k1 = d1.count(" ") - d1.count("لا أستطيع");
        const int k2 = d2.count(" ") - d2.count("لا أستطيع");
        QString best = (k1>=k2?d1:d2).trimmed();
        if(best.isEmpty()) best = "لا أستطيع الإجابة بثقة.";
        return best.left(1200);
    } else {
        if(clean.endsWith("?"))
            return "الوضع السيادي مفعّل والنموذج المحلي غير مهيّأ. لا أستطيع الإجابة بثقة.";
        return "تم استلام الأمر. لا نموذج محلي مفعل حالياً.";
    }
}
