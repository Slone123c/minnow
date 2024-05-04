#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

bool Writer::is_closed() const
{
  return closed_;
}

void Writer::push( string data )
{
  auto len = min( data.size(), available_capacity() ); // 确定可写入的数据长度
  if ( len == 0 ) { // 如果可写入的数据长度为0，说明已经写满了，返回
    return;
  } else if ( len < data.size() ) { // 如果可写入的数据长度小于 data 的长度，说明只能写入部分数据
    data.resize( len );             // 将 data 的长度截断为可写入的长度
  }
  // 将 data 写入到 buffer 中
  buffer_.push( move( data ) );
  if ( buffer_.size() == 1 ) // 写入前为空时需要更新 buffer_view
    buffer_view = buffer_.front();
  // 更新已写入的数据长度
  bytes_pushed_ += len;
}

void Writer::close()
{
  closed_ = true;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  // if (buffer_.size() > capacity_)
  // {
  //   return 0;
  // }

  return capacity_ - reader().bytes_buffered();
}

uint64_t Writer::bytes_pushed() const
{
  return bytes_pushed_;
}

bool Reader::is_finished() const
{
  return closed_ && buffer_.size() == 0;
}

uint64_t Reader::bytes_popped() const
{
  return bytes_popped_;
}

string_view Reader::peek() const
{
  return buffer_view;
}

void Reader::pop( uint64_t len )
{
  // 检查要弹出的长度是否超过了当前缓冲区中的数据量
  if ( len > bytes_buffered() ) {
    return;
  }
  // 更新已弹出的数据总长度
  bytes_popped_ += len;

  // 循环弹出数据，直到弹出的长度达到要求
  while ( len > 0 ) {
    // 如果要弹出的长度大于或等于当前字符串视图的长度，
    // 则需要从队列中移除这个字符串，并更新视图到下一个字符串（如果有的话）
    if ( len >= buffer_view.size() ) {
      len -= buffer_view.size(); // 减去当前视图的长度
      buffer_.pop();             // 从队列中移除已经处理完的字符串
      // 检查队列是否为空，如果不为空，则更新视图到新的队首元素
      if ( !buffer_.empty() ) {
        buffer_view = buffer_.front();
      } else {
        // 如果队列为空，清空视图
        buffer_view = {};
      }
    } else {
      // 如果要弹出的长度小于当前视图的长度，
      // 只需要更新视图的起始位置，即移除视图前面的部分
      buffer_view.remove_prefix( len );
      len = 0; // 已弹出所需长度，结束循环
    }
  }
}

uint64_t Reader::bytes_buffered() const
{
  return writer().bytes_pushed() - bytes_popped();
}
