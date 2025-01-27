/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "http_message.hh"
#include "exception.hh"
#include "http_record.pb.h"

using namespace std;

/* methods called by an external parser */
void HTTPMessage::set_first_line( const string & str )
{
    assert( state_ == FIRST_LINE_PENDING );
    first_line_ = str;
    state_ = HEADERS_PENDING;
}

void HTTPMessage::add_header( const std::string & str )
{
    assert( state_ == HEADERS_PENDING );
    headers_.emplace_back( str );
}

void HTTPMessage::done_with_headers( void )
{
    assert( state_ == HEADERS_PENDING );
    state_ = BODY_PENDING;

    calculate_expected_body_size();
}

void HTTPMessage::set_expected_body_size( const bool is_known, const size_t value )
{
    assert( state_ == BODY_PENDING );
    
    expected_body_size_ = make_pair( is_known, value );
}

size_t HTTPMessage::read_in_body( const std::string & str )
{
    assert( state_ == BODY_PENDING );

    if ( body_size_is_known() ) {
        /* body size known in advance */

        assert( body_.size() <= expected_body_size() );
        const size_t amount_to_append = min( expected_body_size() - body_.size(),
                                             str.size() );

        body_.append( str.substr( 0, amount_to_append ) );
        if ( body_.size() == expected_body_size() ) {
            state_ = COMPLETE;
        }

        return amount_to_append;
    } else {
        /* body size not known in advance */
        return read_in_complex_body( str );
    }
}

void HTTPMessage::eof( void )
{
    switch ( state() ) {
    case FIRST_LINE_PENDING:
        return;
    case HEADERS_PENDING:
        throw runtime_error( "HTTPMessage: EOF received in middle of headers" );
    case BODY_PENDING:
        if ( eof_in_body() ) {
            state_ = COMPLETE;
        }
        break;
    case COMPLETE:
        assert( false ); /* nobody should be calling methods on a complete message */
        return;
    }
}

bool HTTPMessage::body_size_is_known( void ) const
{
    assert( state_ > HEADERS_PENDING );
    return expected_body_size_.first;
}

size_t HTTPMessage::expected_body_size( void ) const
{
    assert( body_size_is_known() );
    return expected_body_size_.second;
}

/* locale-insensitive ASCII conversion */
static char http_to_lower( char c )
{
    const char diff = 'A' - 'a';
    if ( c >= 'A' and c <= 'Z' ) {
        c -= diff;
    }
    return c;
}

static string strip_initial_whitespace( const string & str )
{
    size_t first_nonspace = str.find_first_not_of( " " );
    if ( first_nonspace == std::string::npos ) {
        return "";
    } else {
        return str.substr( first_nonspace );
    }
}

/* check if two strings are equivalent per HTTP 1.1 comparison (case-insensitive) */
bool HTTPMessage::equivalent_strings( const string & a, const string & b )
{
    const string new_a = strip_initial_whitespace( a ),
        new_b = strip_initial_whitespace( b );

    if ( new_a.size() != new_b.size() ) {
        return false;
    }

    for ( auto it_a = new_a.begin(), it_b = new_b.begin(); it_a < new_a.end(); it_a++, it_b++ ) {
        if ( http_to_lower( *it_a ) != http_to_lower( *it_b ) ) {
            return false;
        }
    }

    return true;
}

bool HTTPMessage::has_header( const string & header_name ) const
{
    for ( const auto & header : headers_ ) {
        /* canonicalize header name per RFC 2616 section 2.1 */
        if ( equivalent_strings( header.key(), header_name ) ) {
            return true;
        }
    }

    return false;
}

const string & HTTPMessage::get_header_value( const std::string & header_name ) const
{
    for ( const auto & header : headers_ ) {
        /* canonicalize header name per RFC 2616 section 2.1 */
        if ( equivalent_strings( header.key(), header_name ) ) {
            return header.value();
        }
    }

    throw runtime_error( "HTTPMessage header not found: " + header_name );
}

/* serialize the request or response as one string */
std::string HTTPMessage::str( void ) const
{
    assert( state_ == COMPLETE );

    bool chunked = false;

    /* start with first line */
    string ret( first_line_ + CRLF );

    /* iterate through headers and add "key: value\r\n" to request */
    for ( const auto & header : headers_ ) {
        if (header.str().compare("transfer-encoding: chunked") != 0){
            ret.append( header.str() + CRLF );
        }
        else{
            ret.append( header.str() + CRLF );
            chunked = true;
        }
    }

    /* blank line between headers and body */
    ret.append( CRLF );

    /* add body to request */
    if (chunked == false){
        ret.append( body_ );
    }
    else{
        //ret.append(h2_body());
        ret.append(body_);
    }
    

    return ret;
}

std::string HTTPMessage::h2_body( void ) const
{
    std::string h2_body("");
    std::string cur_string(body_);
    size_t index = 0;
    while (true)
    {
        index = cur_string.find("\r\n");
        std::string chunk_len_b = cur_string.substr(0, index);
        std::string chunk = cur_string.substr(index+2, cur_string.length() - index-2);
        unsigned int chunk_len = std::stoul(chunk_len_b, nullptr, 16);
        if (chunk_len == 0) break;
        h2_body.append(chunk.substr(0, chunk_len));
        cur_string = chunk.substr(chunk_len+2, chunk.length() - chunk_len-2);
    }
    return h2_body;
}

std::string HTTPMessage::ret_body( void ) const
{

    /* start with first line */
    string ret("");
    /* add body to request */
    ret.append( body_ );

    return ret;
}

MahimahiProtobufs::HTTPMessage HTTPMessage::toprotobuf( void ) const
{
    assert( state_ == COMPLETE );

    MahimahiProtobufs::HTTPMessage ret;

    ret.set_first_line( first_line_ );

    for ( const auto & header : headers_ ) {
        ret.add_header()->CopyFrom( header.toprotobuf() );
    }

    ret.set_body( body_ );

    return ret;
}

HTTPMessage::HTTPMessage( const MahimahiProtobufs::HTTPMessage & proto )
    : first_line_( proto.first_line() ),
      body_( proto.body() ),
      state_( COMPLETE )
{
    for ( const auto header : proto.header() ) {
        headers_.emplace_back( header );
    }
}
