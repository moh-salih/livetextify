#pragma once

#include <QObject>
#include "liveTextify/types.h"

class RAGConfigManager: public QObject{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged FINAL)
    Q_PROPERTY(int  minChunkSize READ minChunkSize WRITE setMinChunkSize NOTIFY minChunkSizeChanged FINAL)
    Q_PROPERTY(int  maxChunkSize READ maxChunkSize WRITE setMaxChunkSize NOTIFY maxChunkSizeChanged FINAL)
    Q_PROPERTY(int  topKResults READ topKResults WRITE setTopKResults NOTIFY topKResultsChanged FINAL)
    Q_PROPERTY(int  embeddingDimension READ embeddingDimension WRITE setEmbeddingDimension NOTIFY embeddingDimensionChanged FINAL)
    Q_PROPERTY(qreal minSimilarityScore  READ minSimilarityScore  WRITE setMinSimilarityScore  NOTIFY minSimilarityScoreChanged FINAL)
    Q_PROPERTY(qreal lengthBoostFactor  READ lengthBoostFactor  WRITE setLengthBoostFactor  NOTIFY lengthBoostFactorChanged FINAL)
    Q_PROPERTY(qreal fillerThreshold READ fillerThreshold WRITE setFillerThreshold NOTIFY fillerThresholdChanged FINAL)
public:
    explicit RAGConfigManager(QObject * parent = nullptr);

    // Getters
    int   minChunkSize()       const { return mConfig.chunking.minChunkLength; }
    int   maxChunkSize()       const { return mConfig.chunking.maxChunkLength; }
    int   topKResults()        const { return mConfig.search.topK; }
    int   embeddingDimension() const { return mConfig.index.dim; }
    qreal minSimilarityScore() const { return mConfig.search.minSimilarity; }
    qreal lengthBoostFactor()  const { return mConfig.chunking.lengthBoostFactor; }
    qreal fillerThreshold()    const { return mConfig.chunking.fillerWordLimit; }
    bool enabled() const { return mEnabled; }
    // Setters
    void setEnabled(bool);
    void setMinChunkSize(int);
    void setMaxChunkSize(int);
    void setTopKResults(int);
    void setEmbeddingDimension(int);
    void setMinSimilarityScore(qreal);
    void setLengthBoostFactor(qreal);
    void setFillerThreshold(qreal);


    // State management
    const RagConfig& config() const { return mConfig; }
    Q_INVOKABLE void resetToDefaults();

    void loadFromSettings();
    void saveToSettings();
signals:
    void enabledChanged();
    void minChunkSizeChanged();
    void maxChunkSizeChanged();
    void topKResultsChanged();
    void embeddingDimensionChanged();
    void minSimilarityScoreChanged();
    void lengthBoostFactorChanged();
    void fillerThresholdChanged();
    void configChanged();
private:
    RagConfig mConfig;
    bool mEnabled{};
};

