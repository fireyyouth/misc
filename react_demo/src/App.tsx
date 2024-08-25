import { useState } from 'react'
import './App.css'

interface PostData {
  id: string,
  date: string,
  content: string,
  comments: string[]
}

function uuidv4() {
  return "10000000-1000-4000-8000-100000000000".replace(/[018]/g, c =>
    (+c ^ crypto.getRandomValues(new Uint8Array(1))[0] & 15 >> +c / 4).toString(16)
  );
}

interface PostProps {
  data: PostData,
  removePost: (id: string) => void
}

function Post(props: PostProps) {
  let [opVisible, setOpVisible] = useState(false)
  let [commentList, setCommentList] = useState(Array.from(props.data.comments))
  let [commentVisible, setCommentVisible] = useState(false)
  let [commentStage, setCommentStage] = useState('')

  function opClass() {
    return 'op ' + (opVisible? '' : 'invisible')
  }

  function toggleComment() {
    setCommentVisible(!commentVisible)
  }

  function commitCommentStage() {
    let cp = Array.from(commentList)
    cp.push(commentStage)
    setCommentList(cp)
    setCommentStage('')
  }

  return (
    <div className="post" onMouseOver={() => setOpVisible(true)} onMouseLeave={() => setOpVisible(false)}>
      <h4>{props.data.date}</h4>
      <p>{props.data.content}</p>
      <div className={opClass()}>
        <div onClick={() => props.removePost(props.data.id)}>
          delete
        </div>
        <div onClick={() => toggleComment()}>
          comment
        </div>
      </div>
      {commentVisible &&
        <>
          <div>
            <input value={commentStage} onChange={e => setCommentStage(e.target.value)}></input>
            <div>
              <button onClick={commitCommentStage}>comment</button>
            </div>
          </div>
          <div>
            {commentList.map(c => <div key={uuidv4()}>bot: {c}</div>)}
          </div>
        </>
      }
    </div>
  )
}

function App() {
  const [postList, setPostList] = useState([
    {
      id: uuidv4(),
      date: '10.01',
      content: 'today is a good day',
      comments: [
        "good job",
        "me too"
      ]
    },
    {
      id: uuidv4(),
      date: '10.02',
      content: 'today is a bad day',
      comments: [
        "feel sorry for you",
      ]
    }
  ])

  let [stage, setStage] = useState('')

  function removePost(postId: string) {
    let cp = Array.from(postList)
    for (let i = 0; i < cp.length; ++i) {
      if (cp[i].id == postId) {
        cp.splice(i, 1)
        break
      }
    }
    setPostList(cp)
  }

  function commitStage() {
    let cp = Array.from(postList)
    cp.push({
      id: uuidv4(),
      date: `10.${Math.floor(Math.random() * 10)}`,
      content: stage,
      comments: []
    })
    setStage('')
    setPostList(cp)
  }

  return (
    <>
      <div className="panel">
        {postList.map((e) => <Post key={e.id} data={e} removePost={removePost} ></Post>)}
      </div>
      <div className="stage">
        <textarea value={stage} onChange={e => setStage(e.target.value)}></textarea>
        <button onClick={() => commitStage()}>append</button>
      </div>
    </>
  )
}

export default App
